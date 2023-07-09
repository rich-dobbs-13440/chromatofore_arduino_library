#include <Servo.h>

#ifndef STANDARD_SERVO_H
#define STANDARD_SERVO_H


#include <Servo.h>

class StandardServo : public IServo {
private:
  Servo servo;

public:
  StandardServo(int pin) {
    servo.attach(pin);
  }

  void write(int angle) {
    servo.write(angle);
  }

  void detach() {
    servo.detach();
  }
};


