#include "gcodeSerialHandler.h"

float float_nan = std::numeric_limits<float>::quiet_NaN();

void GcodeSerialHandler::handleSerial() {
  if (!commandBuffer.isEmpty()) {
    GcodeCommand command = commandBuffer.get();
    executeGcodeCommand(command);
  } else if (Serial.available() > 0) {
    if (debug) {
      debugLog("Serial.available() is true");
    }
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
        bufferIndex = 0;
        GcodeCommand gcode_command = processInputBuffer();
        bool success = commandBuffer.put(gcode_command);
        if (!success) {
          debugLog(
              "Internal error: Failed to add command to buffer, despite buffer "
              "supposedly being empty");
        }
      }
    }
  }
}

GcodeCommand GcodeSerialHandler::processInputBuffer() {
  String gcode_line(inputBuffer);
  acknowledgeCommand(gcode_line);
  debugLog("Received ", gcode_line);
  return parseGcodeLine(inputBuffer);
}

GcodeCommand GcodeSerialHandler::parseGcodeLine(const char *format, ...) {
  char input[128];
  va_list args;
  va_start(args, format);
  vsnprintf(input, sizeof(input), format, args);
  va_end(args);

  char *commentMarker = strchr(input, ';');
  if (commentMarker != nullptr) {
    *commentMarker = '\0';  // Truncate the buffer at the comment marker
  }
  if (debug) {
    debugLog("After removal of comment ", input);
  }
  GcodeCommand cmd;

  char *token;
  char delimiter = ' ';
  token = strtok(input, &delimiter);

  while (token != NULL) {
    String word = token;
    if (word.length() >= 2) {  // Ensure the word has at least a letter and a number.
      char letter = word[0];
      float value = word.substring(1).toFloat();
      cmd.set(letter, value);
    }
    token = strtok(NULL, &delimiter);
  }
  return cmd;
}

void GcodeSerialHandler::executeGcodeCommand(const GcodeCommand &cmd) {
  if (!isnan(cmd.get('T'))) {
    int tool = cmd.get('T');
    changer->setCurrentFilament(tool);
  }
  int currentFilament = changer->getCurrentFilament();
  EarwigFilamentActuator *pActuator = changer->getActuator(currentFilament);
  if (!isnan(cmd.get('G'))) {
    switch (int(cmd.get('G'))) {
      case 1:  // Linear interpolation movement
        if (!isnan(cmd.get('B'))) {
          if (pActuator) {
            debugLog("Handle fixed clamp command. Angle:", cmd.get('B'));
            int angle = cmd.get('B');
            pActuator->setFixedClampServoAngle(angle);
          } else {
            debugLog("Can't handle fixed clamp command. Angle:", cmd.get('B'),
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(cmd.get('C'))) {
          if (pActuator) {
            debugLog("Handle clamp command. Angle:", cmd.get('C'));
            int angle = cmd.get('C');
            pActuator->setMovingClampServoAngle(angle);
          } else {
            debugLog("Can't handle clamp command. Angle:", cmd.get('C'),
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(cmd.get('E'))) {
          if (pActuator) {
            debugLog("Handle extrusion command. e:", cmd.get('E'), "f:", cmd.get('F'), "q:", cmd.get('Q'));
            float mm_of_filament = cmd.get('E');
            float feedrate_mm_per_minute = cmd.get('F');
            bool use_filament_detector = !isnan(cmd.get('Q'));
            bool require_filament = cmd.get('G') > 0;
            pActuator->extrude(mm_of_filament, feedrate_mm_per_minute, use_filament_detector, require_filament);
          } else {
            debugLog("Can't handle extrusion command. e:", cmd.get('E'), "f:", cmd.get('F'),
                     "No current actuator found with index:", currentFilament);
          }
        } else if (!isnan(cmd.get('X'))) {
          if (pActuator) {
            debugLog("Handle move command. Angle:", cmd.get('X'));
            int angle = cmd.get('X');
            pActuator->setPusherServoAngle(angle);
          } else {
            debugLog("Can't handle move command. Angle:", cmd.get('X'),
                     "No current actuator found with index:", currentFilament);
          }
        } else {
          debugLog("Unknown G1 command");
        }

        break;
      case 10:  //
        debugLog("Handle programmable data input command");
        {
          int tool = cmd.get('T');
          int choice = cmd.get('L');
          if (choice == 0) {
            changer->rememberMinimumAngleForTool(tool, cmd.get('B'), cmd.get('C'), cmd.get('X'));
          } else if (choice == 1) {
            changer->rememberMaximumAngleForTool(tool, cmd.get('B'), cmd.get('C'), cmd.get('X'));
          } else {
            debugLog("Unrecognized l value for programmable data input command");
          }
        }
        break;
      case 28:
        debugLog("Handle home command");
        // Home axis
        if (pActuator) {
          debugLog("Handle home command. b:", cmd.get('B'), "c:", cmd.get('C'), "x:", cmd.get('X'));
          pActuator->home(cmd.get('B'), cmd.get('C'), cmd.get('X'));
        } else {
          debugLog("Can't handle home command. b:", cmd.get('B'), "c:", cmd.get('C'), "x:", cmd.get('X'),
                   "No current actuator found with index:", currentFilament);
        }
        break;
      default:
        // Code to execute when g doesn't match any case
        debugLog("Value of g doesn't match any case");
        break;
    }
  } else if (!isnan(cmd.get('M'))) {
    switch (int(cmd.get('M'))) {
      case 119:
        if (pActuator) {
          pActuator->printSwitchStates();
        } else {
          debugLog("Can't handle print switch states command M119. t: ", cmd.get('T'),
                   "No current actuator found with index:", currentFilament);
        }
        break;
      case 999:
        // Reset the servo associated with a particular axes, presumably after fixing a problem.
        // M999 T1 B1 C1 X1: 
      case 1001: {
        // Test case 1
        int t = currentFilament;
        float e = isnan(cmd.get('E')) ? 100 : cmd.get('E');
        commandBuffer.put(parseGcodeLine("G1 T%d E-300 Q1", t));  // Retract filament until filament sensor is hit;
        commandBuffer.put(parseGcodeLine("G1 T%d E%f", t, e));    // Extrude forward
        commandBuffer.put(parseGcodeLine("G1 T%d E-300 Q1", t));  // Retract filament until filament sensor is hit;
        commandBuffer.put(parseGcodeLine("G1 T%d E%f", t, e));    // Extrude forward
        commandBuffer.put(parseGcodeLine("G1 T%d E-300 Q1", t));  // Retract filament until filament sensor is hit;
        commandBuffer.put(parseGcodeLine("G1 T%d E%f", t, e));    // Extrude forward
      }

      default:
        // Code to execute when m doesn't match any supported case
        debugLog("Value of m doesn't match any case");
        break;
    }

  } else if (!isnan(cmd.get('T'))) {
    debugLog("Handle tool change command. New tool index:", cmd.get('T'));
    changer->selectNextFilament(cmd.get('T'));
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