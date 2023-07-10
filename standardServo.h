#pragma once

#include <Servo.h>
#include "iServo.h"

#include "debugLog.h"

class StandardServo : public IServo {
 private:
  Servo servo;
  int currentAngle = -1;
  int pin = -1;

 public:
  StandardServo(int pin) : pin(pin) {  }

  void begin() {
    debugLog("StandardServo::begin() called.  Pin", pin);
    servo.attach(pin);
    debugLog("CurrentAngle", currentAngle);
  }

  void write(int angle) {
    currentAngle = angle;
    servo.write(angle);
    debugLog("Pin", pin, "currentAngle", currentAngle);
  }

  void detach() { servo.detach(); }
};
