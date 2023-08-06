/*
This class implements the overall filament changer object.

The filament changer can be configured and controlled by sending it GCODE over
the USB serial connection.

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
const int MOVING_CLAMP_LIMIT_SWITCH = 4;

class ChromatoforeFilamentChanger {
 private:
  
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

  String version() {
    return "0.2l - limit switch homing - reset functionality and isFailed method";
  }

  bool configureForI2C(int actuatorCount, int servoCount, int servoConfiguration[][4], int pinCount, int pinConfiguration[][4]);

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
