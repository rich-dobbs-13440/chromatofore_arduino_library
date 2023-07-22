/*  Provides a concrete implementation of IServo that is based on the Adafruit
   PWM Servo Driver library.

    In addition to this libary, you will need to install the Adafruit BusIO
   library too.

    It is used for servos that are connected by plugging into this expansion
   board, which is controlled via I2C communication.

*/

#pragma once

#include <Arduino.h>

#include "debugLog.h"
#include "iServo.h"
#include "pca9685ServoDriver.h"

struct Pca9685ServoInfo {
  int servoIndex;
  int i2cAddress;
  int pin;
};

Pca9685ServoInfo getPca9685ServoInfo(int servoConfiguration[][4], int numRows,
                                     int actuator, int role);

class Pca9685PinServo : public IServo {
 private:
  String id;
  PCA9685ServoDriver* servoDriver;
  int pin = -1;
  int minimumAngle = -1;
  int maximumAngle = -1;
  int currentAngle = -1;

 public:
  Pca9685PinServo() {}

  void initialize(String id, PCA9685ServoDriver* servoDriver, int pin) {
    this->id = id;
    this->servoDriver = servoDriver;
    this->pin = pin;
  }

  void begin(int minimumAngle, int maximumAngle,
             float initialRelativePosition) {
    debugLog("Pca9685PinServo::begin() called.  id: ", id, "Pin", pin, "minimumAngle",
             minimumAngle, "maximumAngle", maximumAngle,
             "initialRelativePosition", initialRelativePosition);
    this->minimumAngle = minimumAngle;
    this->maximumAngle = maximumAngle;
    position(initialRelativePosition);
    debugLog("CurrentAngle", currentAngle);
    delay(100);
    servoDriver->atEase(pin);
  }

  void write(int angle) {
    currentAngle = angle;
    servoDriver->setServoAngle(pin, angle);
  }

  void position(float relativePosition) {
    int angle = minimumAngle + relativePosition * (maximumAngle - minimumAngle);
    write(angle);
  }
  void detach() {}

  void dump() {
    debugLog("id:", id, "pin:", pin, "minimumAngle:", minimumAngle,
             "maximumAngle:", maximumAngle, "currentAngle:", currentAngle);
  }

  void atEase() { servoDriver->atEase(pin); }
};
