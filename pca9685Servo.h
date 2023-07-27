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
#include "homerServo.h"
#include "pca9685ServoDriver.h"

struct Pca9685ServoInfo {
  int servoIndex;
  int i2cAddress;
  int pin;
};

Pca9685ServoInfo getPca9685ServoInfo(int servoConfiguration[][4], int numRows,
                                     int actuator, int role);

class Pca9685PinServo : public HomerServo {
 private:
  String id;
  PCA9685ServoDriver* servoDriver;
  int pin = -1;


 public:
  Pca9685PinServo() {}

  void initialize(String id, PCA9685ServoDriver* servoDriver, int pin) {
    this->id = id;
    this->servoDriver = servoDriver;
    this->pin = pin;
  }

  void begin(int minimumAngle, int maximumAngle,
             float initialRelativePosition) {
    /*
    debugLog("Pca9685PinServo::begin() called.  id: ", id, "Pin", pin, "minimumAngle",
             minimumAngle, "maximumAngle", maximumAngle,
             "initialRelativePosition", initialRelativePosition);    
    */

    setMinimumAngle(minimumAngle);
    setMaximumAngle(maximumAngle);
    auto expectedArrivalTime = position(initialRelativePosition);
    // debugLog("CurrentAngle", currentAngle);
    delay(expectedArrivalTime-millis());
    servoDriver->atEase(pin);
  }

  ExpectedArrivalMillis write(int angle) override {
    currentAngle = angle;
    servoDriver->setServoAngle(pin, angle);
    // For now, just use a constant delay.  It would be better to base this on how much the servo moves and the speed of the servo.  
    return millis() + movementDelayMillis; 
  }


  void detach() {}

  void dump() {
    debugLog("id:", id, "pin:", pin, "minimumAngle:", getMinimumAngle(),
             "maximumAngle:", getMaximumAngle(), "currentAngle:", currentAngle);
  }

  void atEase() { servoDriver->atEase(pin); }

};
