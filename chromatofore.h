/* 
This class implements the overall filament changer object.

The filament changer can be configured and controlled by sending it GCODE over the USB serial
connection.  

The axes used for the machine for the Earwig Filament Actuator are:

      Rotation of fixed clamp:  B
      Rotation of moving clamp: C
      Location of moving clamp: X

Note that at this time, actual location of the moving clamp is not linear with the X, 
but is a function of the linkage kinematics.  

The following GCODE parsing is currently implemented:


G1 B10 ; Move the fixed clamp servo to an angle of 10 degrees 

G1 C10 ; Move the clamp servo to an angle of 10 degrees

G1 X10 ; Move the pusher servo to an angle of 10 degrees

G10 T1 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles for tool 1

G10 T2 L1 B101 C101 X145 ; Remember maximum values for the B, C, and X axis angles for tool 2

G28 B C X ; Home all axes, say for loading or unloading filament.

The following commands are parsed, but the functionality is not yet implemented:


G1 E10 F10 : Extrude 10 mm of filament, feed rate currently ignore.  (Parsed by functionality is not yet implemented in the actuator!)


*/


#pragma once

#include <Arduino.h>
#include <earwig.h>
#include <string.h>



class ChromatoforeFilamentChanger {
 private:
  String version = "0.1t";
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
  bool echoCharacters = false;

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

  int getMinimumAngleB(int tool) const;
  int getMinimumAngleC(int tool) const;
  int getMinimumAngleX(int tool) const;

  int getMaximumAngleB(int tool) const;
  int getMaximumAngleC(int tool) const;
  int getMaximumAngleX(int tool) const;

  int getActuatorArraySize() {return actuatorArraySize; } 

  void initializeEEPROM();
};
