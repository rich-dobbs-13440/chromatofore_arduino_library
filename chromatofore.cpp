#include "chromatofore.h"

#include <EEPROM.h>

#include "debugLog.h"

float nan = sqrt(-1);

ChromatoforeFilamentChanger::ChromatoforeFilamentChanger(int size)
    : actuatorArraySize(size) {
  actuatorArray = new EarwigFilamentActuator *[actuatorArraySize]();
}

ChromatoforeFilamentChanger::~ChromatoforeFilamentChanger() {
  delete[] actuatorArray;
}

void ChromatoforeFilamentChanger::addActuator(
    int index, EarwigFilamentActuator *actuator) {
  if (index >= 0 && index < actuatorArraySize) {
    actuatorArray[index] = actuator;
  }
}

void ChromatoforeFilamentChanger::removeActuator(int index) {
  if (index >= 0 && index < actuatorArraySize) {
    actuatorArray[index] = nullptr;
  }
}

EarwigFilamentActuator *ChromatoforeFilamentChanger::getActuator(int index) {
  if (index >= 0 && index < actuatorArraySize) {
    return actuatorArray[index];
  }
  return nullptr;
}

void ChromatoforeFilamentChanger::begin() {
  Serial.begin(baudRate);

  debugLog("--------------");
  debugLog("Chromatofore Version:", version);
  debugLog("Upload Date:", __DATE__);
  debugLog("Upload Time:", __TIME__);
  debugLog("Baud Rate:", baudRate);

  int restartCount = EEPROM.read(0);
  debugLog("Restart Count:", restartCount);
  restartCount++;
  EEPROM.write(0, restartCount);

  debugLog("Millis:", millis());

  for (int tool = 0; tool < actuatorArraySize; ++tool) {
    if (actuatorArray[tool] != nullptr) {
      uint8_t minB = getMinimumAngleB(tool);
      uint8_t maxB = getMaximumAngleB(tool);
      uint8_t minC = getMinimumAngleC(tool);
      uint8_t maxC = getMaximumAngleC(tool);
      uint8_t minX = getMinimumAngleX(tool);
      uint8_t maxX = getMaximumAngleX(tool);
      actuatorArray[tool]->begin(minB, maxB, minC, maxC, minX, maxX);
    }
  }
}

void ChromatoforeFilamentChanger::loop() { handleSerial(); }

void ChromatoforeFilamentChanger::handleSerial() {
  if (Serial.available() > 0) {
    debugLog("Serial.available() is true");
    while (Serial.available() > 0) {
      char serialChar = Serial.read();
      if (echoCharacters) debugLog("Received char", serialChar, int(serialChar));

      if (serialChar != '\n' && serialChar != '\r') {
        // Add character to the buffer
        if (bufferIndex >= BUFFER_SIZE - 1) {
          debugLog("Buffer full, ignoring character ", serialChar);
        } else {
          inputBuffer[bufferIndex] = serialChar;
          bufferIndex++;
        }
      } else {
        // Line ending character encountered
        inputBuffer[bufferIndex] = '\0';  // Null-terminate the buffer
        bufferIndex = 0;                  // Reset buffer index
        processInputBuffer();             // Process the received line
      }
    }
  }
}

void ChromatoforeFilamentChanger::processInputBuffer() {
  String gcode_line(inputBuffer);
  acknowledgeCommand(gcode_line);
  debugLog("Received ", gcode_line);
  char *commentMarker = strchr(inputBuffer, ';');
  if (commentMarker != nullptr) {
    *commentMarker = '\0';  // Truncate the buffer at the comment marker
  }
  debugLog("After removal of comment ", inputBuffer);

  char *token;
  char delimiter = ' ';
  token = strtok(inputBuffer, &delimiter);

  float b = nan;
  float c = nan;
  float e = nan;
  float f = nan;
  float g = nan;
  float l = nan;
  float t = nan;
  float x = nan;

  // debugLog("e", e);
  while (token != NULL) {
    String word = token;
    // debugLog("word", word);
    if (word.startsWith("B")) {
      b = word.substring(1).toFloat();
    }
    if (word.startsWith("C")) {
      c = word.substring(1).toFloat();
    } else if (word.startsWith("E")) {
      e = word.substring(1).toFloat();
    } else if (word.startsWith("E")) {
      e = word.substring(1).toFloat();
    } else if (word.startsWith("F")) {
      f = word.substring(1).toFloat();
    } else if (word.startsWith("G")) {
      g = word.substring(1).toFloat();
    } else if (word.startsWith("L")) {
      l = word.substring(1).toFloat();
    } else if (word.startsWith("T")) {
      t = word.substring(1).toInt();
    } else if (word.startsWith("X")) {
      x = word.substring(1).toFloat();
    }
    token = strtok(NULL, &delimiter);
  }
  // debugLog("c:", c);
  // debugLog("e:", e);
  // debugLog("f:", f);
  // debugLog("g:", g);

  if (!isnan(g)) {
    switch (int(g)) {
      case 1:  // Linear interpolation movement
        if (!isnan(b)) {
          debugLog("Handle fixed clamp command. Angle:", b);
          EarwigFilamentActuator *pActuator = getActuator(currentFilament);
          if (pActuator) {
            int angle = b;
            pActuator->setFixedClampServoAngle(angle);
          } else {
            debugLog("No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(c)) {
          debugLog("Handle clamp command. Angle:", c);
          EarwigFilamentActuator *pActuator = getActuator(currentFilament);
          if (pActuator) {
            int angle = c;
            pActuator->setMovingClampServoAngle(angle);
          } else {
            debugLog("No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(e)) {
          debugLog("Handle extrusion command.");
          float mm_of_filament = e;
          float feedrate_mm_per_minute = f;
          // extrude(mm_of_filament, feedrate_mm_per_minute);
        } else if (!isnan(x)) {
          debugLog("Handle move command. Angle:", x);
          EarwigFilamentActuator *pActuator = getActuator(currentFilament);
          if (pActuator) {
            int angle = x;
            pActuator->setPusherServoAngle(angle);
          } else {
            debugLog("No current actuator found with index:", currentFilament);
          }
        } else {
          debugLog("Unknown G1 command");
        }

        break;
      case 10:  //
        debugLog("Handle programmable data input command");
        int tool = t;
        int choice = l;
        if (choice == 0) {
          rememberMinimumAngleForTool(tool, b, c, x);
        } else if (choice == 1) {
          rememberMaximumAngleForTool(tool, b, c, x);
        } else {
          debugLog("Unrecognize l value for programmable data input command");
        }
        break;
      case 28:
        // Home axis
        if (c != 0) {
          // home_clamp_servo();
        }
        break;
      default:
        // Code to execute when g doesn't match any case
        debugLog("Value of g doesn't match any case");
        break;
    }
  } else if (!isnan(t)) {
    debugLog("Handle tool change command. New tool index:", t);
    selectNextFilament(t);
  }
}

void ChromatoforeFilamentChanger::acknowledgeCommand(const String &command) {
  // Calculate the checksum
  byte checksum = calculateChecksum(command);

  Serial.print("OK");
  Serial.print(" ");
  Serial.println(checksum, HEX);
  Serial.flush();
}

byte ChromatoforeFilamentChanger::calculateChecksum(const String &command) {
  byte checksum = 0;
  for (size_t i = 0; i < command.length(); i++) {
    checksum ^= command[i];
  }
  return checksum;
}

void ChromatoforeFilamentChanger::rememberMinimumAngleForTool(int tool, float b,
                                                              float c,
                                                              float x) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In rememberMaximumAngleForTool, invalid tool index", tool,
             "No values remembered");
    return;
  }

  // Calculate the EEPROM offset based on the tool index
  int offset = TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6);

  // Check if b value is specified and store it
  if (!isnan(b)) {
    if (b >= 0 && b <= 180) {
      uint8_t minB = static_cast<uint8_t>(b);
      EEPROM.put(offset, minB);
      debugLog("Current b mininum", getMinimumAngleB(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, b is out of range 0-180", b,
               "No value remembered");
    }
  }

  // Check if c value is specified and store it
  if (!isnan(c)) {
    if (c >= 0 && c <= 180) {
      uint8_t minC = static_cast<uint8_t>(c);
      EEPROM.put(offset + sizeof(uint8_t), minC);
      debugLog("Current c mininum", getMinimumAngleC(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, c is out of range 0-180", c,
               "No value remembered");
    }
  }

  // Check if x value is specified and store it
  if (!isnan(x)) {
    if (x >= 0 && x <= 180) {
      uint8_t minX = static_cast<uint8_t>(x);
      EEPROM.put(offset + sizeof(uint8_t) * 2, minX);
      debugLog("Current x mininum", getMinimumAngleX(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, x is out of range 0-180", x,
               "No value remembered");
    }
  }
}

void ChromatoforeFilamentChanger::rememberMaximumAngleForTool(int tool, float b,
                                                              float c,
                                                              float x) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In rememberMaximumAngleForTool, invalid tool index", tool,
             "No values remembered");
    return;
  }
  // Calculate the EEPROM offset based on the tool index
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 3;

  // Check if b value is specified and store it
  if (!isnan(b)) {
    if (b >= 0 && b <= 180) {
      uint8_t maxB = static_cast<uint8_t>(b);
      EEPROM.put(offset, maxB);
      debugLog("Current b maximum", getMaximumAngleB(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, b is out of range 0-180", b,
               "No value remembered");
    }
  }

  // Check if c value is specified and store it
  if (!isnan(c)) {
    if (c >= 0 && c <= 180) {
      uint8_t maxC = static_cast<uint8_t>(c);
      EEPROM.put(offset + sizeof(uint8_t), maxC);
      debugLog("Current c maximum", getMaximumAngleC(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, c is out of range 0-180", c,
               "No value remembered");
    }
  }

  // Check if x value is specified and store it
  if (!isnan(x)) {
    if (x >= 0 && x <= 180) {
      uint8_t maxX = static_cast<uint8_t>(x);
      EEPROM.put(offset + sizeof(uint8_t) * 2, maxX);
      debugLog("Current x maximum", getMaximumAngleX(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, x is out of range 0-180", x,
               "No value remembered");
    }
  }
}

int ChromatoforeFilamentChanger::getMinimumAngleB(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleB, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset = TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6);

  // Retrieve and return the stored minimum angle for axis B
  uint8_t b;
  EEPROM.get(offset, b);
  return b;
}

int ChromatoforeFilamentChanger::getMinimumAngleC(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleC, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t);

  // Retrieve and return the stored minimum angle for axis C
  uint8_t c;
  EEPROM.get(offset, c);
  return c;
}

int ChromatoforeFilamentChanger::getMinimumAngleX(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleX, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 2;

  // Retrieve and return the stored minimum angle for axis X
  uint8_t x;
  EEPROM.get(offset, x);
  return x;
}

int ChromatoforeFilamentChanger::getMaximumAngleB(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleB, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 3;

  // Retrieve and return the stored maximum angle for axis B
  uint8_t b;
  EEPROM.get(offset, b);
  return b;
}

int ChromatoforeFilamentChanger::getMaximumAngleC(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleC, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 4;

  // Retrieve and return the stored maximum angle for axis C
  uint8_t c;
  EEPROM.get(offset, c);
  return c;
}

int ChromatoforeFilamentChanger::getMaximumAngleX(int tool) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleX, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 5;

  // Retrieve and return the stored maximum angle for axis X
  uint8_t x;
  EEPROM.get(offset, x);
  return x;
}

void ChromatoforeFilamentChanger::initializeEEPROM() {
  // Initialize the initial block to zero
  for (int i = 0; i < TOOL_EEPROM_OFFSET; i++) {
    EEPROM.write(i, 0);
  }

  // Initialize each tool with named constants for each axis
  for (int tool = 0; tool < actuatorArraySize; tool++) {
    rememberMinimumAngleForTool(tool, DEFAULT_MINIMUM_ANGLE_B,
                                DEFAULT_MINIMUM_ANGLE_C,
                                DEFAULT_MINIMUM_ANGLE_X);
    rememberMaximumAngleForTool(tool, DEFAULT_MAXIMUM_ANGLE_B,
                                DEFAULT_MAXIMUM_ANGLE_C,
                                DEFAULT_MAXIMUM_ANGLE_X);
  }
}
