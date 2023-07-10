#pragma once

#include <Arduino.h>
#include <earwig.h>
#include <string.h>



class ChromatoforeFilamentChanger {
 private:
  String version = "0.1k";
  EarwigFilamentActuator** actuatorArray;
  int actuatorArraySize;
  int baudRate = 9600;
  static const int BUFFER_SIZE = 256;  // Size of the input buffer
  char inputBuffer[BUFFER_SIZE];       // Input buffer to store characters

  // EEPROM constants
  static const int TOOL_EEPROM_OFFSET = 100;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_B = 10;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_C = 10;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_X = 10;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_B = 100;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_C = 100;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_X = 140;

  int bufferIndex = 0;       // Index to keep track of the buffer position
  int currentFilament = -1;  // None selected
  int nextFilament = -1;     // None selected

  void handleSerial();
  void processInputBuffer();
  void acknowledgeCommand(const String& command);
  byte calculateChecksum(const String& command);

 public:
  ChromatoforeFilamentChanger(int size = 64);
  ~ChromatoforeFilamentChanger();

  void begin();
  void loop();

  void addActuator(int index, EarwigFilamentActuator* actuator);
  void removeActuator(int index);
  EarwigFilamentActuator* getActuator(int index);
  void selectNextFilament(int index) { nextFilament = index; }
  void setCurrentFilament(int index) { currentFilament = index; }
  void rememberMinimumAngleForTool(int tool, float b, float c, float x);
  void rememberMaximumAngleForTool(int tool, float b, float c, float x);

  int getMinimumAngleB(int tool);
  int getMinimumAngleC(int tool);
  int getMinimumAngleX(int tool);

  int getMaximumAngleB(int tool);
  int getMaximumAngleC(int tool);
  int getMaximumAngleX(int tool);

  int getActuatorArraySize() {return actuatorArraySize; } 

  void initializeEEPROM();
};
