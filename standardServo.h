#pragma once

#include <Servo.h>

class StandardServo : public IServo {
 private:
  Servo servo;
  int currentAngle = -1;
  int pin = -1;

 public:
  StandardServo(int pin) : pin(pin) {}

  void begin() { servo.attach(pin); }

  void write(int angle) {
    currentAngle = angle;
    servo.write(angle);
  }

  void detach() { servo.detach(); }
};
