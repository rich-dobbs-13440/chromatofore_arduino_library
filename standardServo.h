#pragma once

#include <Servo.h>

#include "debugLog.h"
#include "iServo.h"

class StandardServo : public IServo {
 private:
  String id;
  Servo servo;
  int pin = -1;
  int minimumAngle = -1;
  int maximumAngle = -1;    
  int currentAngle = -1;



 public:
  StandardServo(String id, int pin) : id(id), pin(pin) {}

  void begin(int minimumAngle, int maximumAngle,
             float initialRelativePosition) {
    debugLog("StandardServo::begin() called.  Pin", pin, "minimumAngle",
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
    debugLog("id:", id, "pin:", pin, "minimumAngle:", minimumAngle, "maximumAngle:", maximumAngle, "currentAngle:", currentAngle);
  }
};
