/*  Provides a concrete implementation of IServo that is based on the Adafruit
   PWM Servo Driver library.

    In addition to this libary, you will need to install the Adafruit BusIO
   library too.

    It is used for servos that are connected by plugging into this expansion
   board, which is controlled via I2C communication.

*/

#pragma once

#include <Adafruit_PWMServoDriver.h>

#include "Arduino.h"
#include "debugLog.h"
#include "iServo.h"

class Pca9685PinServo : public IServo {
 private:
  String id;
  Adafruit_PWMServoDriver& servoDriver;
  int pin = -1;
  int minimumAngle = -1;
  int maximumAngle = -1;
  int currentAngle = -1;

 public:
  Pca9685PinServo(String id, Adafruit_PWMServoDriver& servoDriver, int pin)
      : id(id), servoDriver(boaservoDriverrd), pin(pin) {}

  void begin(int minimumAngle, int maximumAngle,
             float initialRelativePosition) {
    debugLog("Pca9685PinServo::begin() called.  Pin", pin, "minimumAngle",
             minimumAngle, "maximumAngle", maximumAngle,
             "initialRelativePosition", initialRelativePosition);
    this->minimumAngle = minimumAngle;
    this->maximumAngle = maximumAngle;
    servo.attach(pin);
    position(initialRelativePosition);
    debugLog("CurrentAngle", currentAngle);
  }

  void write(int angle) {
    currentAngle = angle;
    servo.write(angle);
  }

  void position(float relativePosition) {
    int angle = minimumAngle + relativePosition * (maximumAngle - minimumAngle);
    write(angle);
  }
  void detach() { servo.detach(); }

  void dump() {
    debugLog("id:", id, "pin:", pin, "minimumAngle:", minimumAngle,
             "maximumAngle:", maximumAngle, "currentAngle:", currentAngle);
  }
};
