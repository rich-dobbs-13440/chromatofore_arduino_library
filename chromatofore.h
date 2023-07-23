/*
This class implements the overall filament changer object.

The filament changer can be configured and controlled by sending it GCODE over
the USB serial connection.

The axes used for the machine for the Earwig Filament Actuator are:

      Rotation of fixed clamp:  B
      Rotation of moving clamp: C
      Location of moving clamp: X

Note that at this time, actual location of the moving clamp is not linear with
the X, but is a function of the linkage kinematics.

The following GCODE parsing is currently implemented:


G1 T0 B10 ; For actuator 0, move the fixed clamp servo to an angle of 10 degrees

G1 T1 C10 ; For actuator 1, Move the clamp servo to an angle of 10 degrees

G1 T2 X10 ; For actuator 2, move the pusher servo to an angle of 10 degrees

G10 T3 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles
for tool 3

G10 T0 L1 B101 C101 X145 ; Remember maximum values for the B, C, and X axis
angles for tool 0

G28 T1 B C X ; Home all axes, say for loading or unloading filament.

The following commands are parsed, but the functionality is not yet implemented:


G1 T1 E10 F10 : Extrude 10 mm of filament, feed rate currently ignore.  (Parsed
by functionality is not yet implemented in the actuator!)


M119 T0; Report the endstop state - at this time just whether filament is detected or not.

*/

#pragma once

#include <Arduino.h>
#include <string.h>

#include "earwig.h"
#include "i2cConfiguration.h"
#include "pca9685Servo.h"
#include "pcf8574Switch.h"
#include "pcf8574FilamentDetector.h"
#include "iSerialHandler.h"

const int PUSHER = 0;
const int MOVING_CLAMP = 1;
const int FIXED_CLAMP = 2;

const int FILAMENT_DETECTOR = 3;

class ChromatoforeFilamentChanger {
 private:
  String version = "0.1t";
  EarwigFilamentActuator** actuatorArray;
  int actuatorArraySize;
  int baudRate = 9600;
  ISerialHandler* serialHandler = nullptr;


  // EEPROM constants
  static const int TOOL_EEPROM_OFFSET = 100;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_B = 10;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_C = 10;
  static const uint8_t DEFAULT_MINIMUM_ANGLE_X = 10;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_B = 100;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_C = 100;
  static const uint8_t DEFAULT_MAXIMUM_ANGLE_X = 140;


  int currentFilament = -1;  // None selected
  int nextFilament = -1;     // None selected
  

  I2CConfiguration* i2cConfiguration = nullptr;
  EarwigFilamentActuator* i2cActuators = nullptr;
  Pca9685PinServo* i2cServos = nullptr;
  Pcf8574FilamentDetector* i2cFilamentDetectors = nullptr;
  int i2cActuatorCount = 0;
  int i2cServoCount = 0;


 public:
  ChromatoforeFilamentChanger(int size = 64);
  ~ChromatoforeFilamentChanger();

  bool configureForI2C(int i2cActuatorCount, 
                        int servoConfiguration[][4], 
                        int gpioConfiguration[][4]);

  void begin();
  void loop();

  void addActuator(int index, EarwigFilamentActuator* actuator);
  void removeActuator(int index);
  EarwigFilamentActuator* getActuator(int index);
  void selectNextFilament(int index) { nextFilament = index; }
  void setCurrentFilament(int index) { currentFilament = index; }
  int getCurrentFilament() {
      return currentFilament;
  }
  void rememberMinimumAngleForTool(int tool, float b, float c, float x);
  void rememberMaximumAngleForTool(int tool, float b, float c, float x);

  int getMinimumAngleB(int tool) const;
  int getMinimumAngleC(int tool) const;
  int getMinimumAngleX(int tool) const;

  int getMaximumAngleB(int tool) const;
  int getMaximumAngleC(int tool) const;
  int getMaximumAngleX(int tool) const;

  int getActuatorArraySize() { return actuatorArraySize; }

  void initializeEEPROM();
};
