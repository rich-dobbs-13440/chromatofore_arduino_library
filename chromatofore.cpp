#include "chromatofore.h"
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
  debugLog("Upload Date: ", __DATE__);
  debugLog("Upload Time: ", __TIME__);
  debugLog("Baud Rate: ", baudRate);
  debugLog("Millis: ", millis());

  for (int i = 0; i < actuatorArraySize; ++i) {
    if (actuatorArray[i] != nullptr) {
      actuatorArray[i]->begin();
    }
  }
}

void ChromatoforeFilamentChanger::loop() { handleSerial(); }

void ChromatoforeFilamentChanger::handleSerial() {
  if (Serial.available() > 0) {
    char serialChar = Serial.read();
    debugLog("Received char", serialChar);

    if (serialChar != '\n' && serialChar != '\r') {
      // Add character to the buffer
      inputBuffer[bufferIndex] = serialChar;
      bufferIndex++;

      // Check if buffer is full
      if (bufferIndex >= BUFFER_SIZE - 1) {
        inputBuffer[bufferIndex] = '\0';  // Null-terminate the buffer
        bufferIndex = 0;                  // Reset buffer index
        processInputBuffer();             // Process the received line
      }
    } else {
      // Line ending character encountered
      inputBuffer[bufferIndex] = '\0';  // Null-terminate the buffer
      bufferIndex = 0;                  // Reset buffer index
      processInputBuffer();             // Process the received line
    }
  }
}

void ChromatoforeFilamentChanger::processInputBuffer() {
  String gcode_line(inputBuffer);
  acknowledgeCommand(gcode_line);
  debugLog("Received ", gcode_line);
  char *token;
  char delimiter = ' ';
  token = strtok(inputBuffer, &delimiter);

  float b = nan;
  float c = nan;
  float e = nan;
  float f = nan;
  float g = nan;
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
      case 1:
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
      case 10:
        // Code to execute when g is 10.0
        debugLog("Value of g is 10");
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
