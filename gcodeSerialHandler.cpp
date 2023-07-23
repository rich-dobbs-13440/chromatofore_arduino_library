#include "gcodeSerialHandler.h"

float float_nan = std::numeric_limits<float>::quiet_NaN();


void GcodeSerialHandler::handleSerial() {
  if (Serial.available() > 0) {
    debugLog("Serial.available() is true");
    while (Serial.available() > 0) {
      char serialChar = Serial.read();
      if (echoCharacters)
        debugLog("Received char", serialChar, int(serialChar));

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

void GcodeSerialHandler::processInputBuffer() {
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

  float b = float_nan;
  float c = float_nan;
  float e = float_nan;
  float f = float_nan;
  float g = float_nan;
  float l = float_nan;
  float m = float_nan;
  float t = float_nan;
  float x = float_nan;

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
    } else if (word.startsWith("M")) {
      m = word.substring(1).toFloat();
    } else if (word.startsWith("T")) {
      t = word.substring(1).toInt();
    } else if (word.startsWith("X")) {
      x = word.substring(1).toFloat();
    }
    token = strtok(NULL, &delimiter);
  }
  debugLog("b:", b);
  debugLog("c:", c);
  // debugLog("e:", e);
  // debugLog("f:", f);
  debugLog("g:", g);
  debugLog("t:", t);
  debugLog("x:", x);

  if (!isnan(t)) {
    int tool = t;
    changer->setCurrentFilament(tool);
  }
  int currentFilament = changer->getCurrentFilament();
  EarwigFilamentActuator *pActuator = changer->getActuator(currentFilament);
  if (!isnan(g)) {
    switch (int(g)) {
      case 1:  // Linear interpolation movement
        if (!isnan(b)) {
          if (pActuator) {
            debugLog("Handle fixed clamp command. Angle:", b);
            int angle = b;
            pActuator->setFixedClampServoAngle(angle);
          } else {
            debugLog("Can't handle fixed clamp command. Angle:", b,
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(c)) {
          if (pActuator) {
            debugLog("Handle clamp command. Angle:", c);
            int angle = c;
            pActuator->setMovingClampServoAngle(angle);
          } else {
            debugLog("Can't handle clamp command. Angle:", c,
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(e)) {
          if (pActuator) {
            debugLog("Handle extrusion command. e:", e, "f:", f);
            float mm_of_filament = e;
            float feedrate_mm_per_minute = f;
            pActuator->extrude(mm_of_filament, feedrate_mm_per_minute);
          } else {
            debugLog("Can't handle extrusion command. e:", e, "f:", f,
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(x)) {
          if (pActuator) {
            debugLog("Handle move command. Angle:", x);
            int angle = x;
            pActuator->setPusherServoAngle(angle);
          } else {
            debugLog("Can't handle move command. Angle:", x,
                     "No current actuator found with index:", currentFilament);
          }
        } else {
          debugLog("Unknown G1 command");
        }

        break;
      case 10:  //
        debugLog("Handle programmable data input command");
        {
          int tool = t;
          int choice = l;
          if (choice == 0) {
            changer->rememberMinimumAngleForTool(tool, b, c, x);
          } else if (choice == 1) {
            changer->rememberMaximumAngleForTool(tool, b, c, x);
          } else {
            debugLog(
                "Unrecognized l value for programmable data input command");
          }
        }
        break;
      case 28:
        debugLog("Handle home command");
        // Home axis
        if (pActuator) {
          debugLog("Handle home command. b:", b, "c:", c, "x:", x);
          pActuator->home(b, c, x);
        } else {
          debugLog("Can't handle home command. b:", b, "c:", c, "x:", x,
                   "No current actuator found with index:", currentFilament);
        }
        break;
      default:
        // Code to execute when g doesn't match any case
        debugLog("Value of g doesn't match any case");
        break;
    }
  } else if (!isnan(m)) {
    switch (int(m)) {
      case 119:
        if (pActuator) {
          pActuator->printSwitchStates();
        } else {
          debugLog("Can't handle home command. b:", b, "c:", c, "x:", x,
                   "No current actuator found with index:", currentFilament);
        }
        break;
      default:
        // Code to execute when m doesn't match any supported case
        debugLog("Value of m doesn't match any case");
        break;
    }

  } else if (!isnan(t)) {
    debugLog("Handle tool change command. New tool index:", t);
    changer->selectNextFilament(t);
  }
}

void GcodeSerialHandler::acknowledgeCommand(const String &command) {
  // Calculate the checksum
  byte checksum = calculateChecksum(command);

  Serial.print("OK");
  Serial.print(" ");
  Serial.println(checksum, HEX);
  Serial.flush();
}

byte GcodeSerialHandler::calculateChecksum(const String &command) {
  byte checksum = 0;
  for (size_t i = 0; i < command.length(); i++) {
    checksum ^= command[i];
  }
  return checksum;
}