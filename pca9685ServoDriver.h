
#pragma once

#include <Adafruit_PWMServoDriver.h>
#include "iBoard.h"

class PCA9685ServoDriver : public iBoard {
 public:
  PCA9685ServoDriver(byte i2cAddress) : i2cAddress(i2cAddress) {}
  void initialize() override {
    // Implementation specific to PCA9685 Servo Driver board initialization
    // Initialize servo driver board here
    pwmServoDriver = Adafruit_PWMServoDriver(i2cAddress);
    pwmServoDriver.begin();
    //pwmServoDriver.setOscillatorFrequency(27000000);
    pwmServoDriver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  }
  void printConfig() override {
    Serial.print("PCA9685ServoDriver, I2C address: 0x");
    Serial.println(i2cAddress, HEX);
  }
  bool setServoAngle(int pinNumber, int angle) {
    if (pinNumber >= 0 && pinNumber < 16) {
      if (angle >= 0 && angle <= 180) {
        int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
        Serial.print("Pulse: ");
        Serial.println(pulse);
        pwmServoDriver.setPWM(pinNumber, 0, pulse);
        return true;
      } else {
        Serial.println("Bad angle");
        return false;
      }
    } else {
       Serial.println("Bad pinNumber");
      return false;
    }
  }
 private:
  byte i2cAddress;
  Adafruit_PWMServoDriver pwmServoDriver;

  // Depending on your servo make, the pulse width min and max may vary, you
  // want these to be as small/large as possible without hitting the hard stop
  // for max range. You'll have to tweak them as necessary to match the servos
  // you have!
  static const int SERVOMIN =
      150;  // This is the 'minimum' pulse length count (out of 4096)
  static const int SERVOMAX =
      600;  // This is the 'maximum' pulse length count (out of 4096)
  static const int USMIN = 600;   // This is the rounded 'minimum' microsecond
                                  // length based on the minimum pulse of 150
  static const int USMAX = 2400;  // This is the rounded 'maximum' microsecond
                                  // length based on the maximum pulse of 600
  static const int SERVO_FREQ = 50;  // Analog servos run at ~50 Hz updates
};