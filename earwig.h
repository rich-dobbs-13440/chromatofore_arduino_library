#pragma once

#include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>
#include "iServo.h"

class EarwigFilamentActuator {
 public:
  EarwigFilamentActuator();
  
  void initialize(IServo& pusherServo, IServo& movingClampServo,
                         IServo& fixedClampServo) {
                          this->pusherServo = &pusherServo;
                          this->movingClampServo = &movingClampServo;
                          this->fixedClampServo = &fixedClampServo;

                         }

  void begin(int minimumFixedClampServoAngle, int maximumFixedClampServoAngle,
             int minimumMovingClampServoAngle, int maximumMovingClampServoAngle,
             int minumPusherServoAngle, int maximumPusherServoAngle);
  void loop();
  void setPusherServoAngle(int angle) { pusherServo->write(angle); }
  void setMovingClampServoAngle(int angle) { movingClampServo->write(angle); }
  void setFixedClampServoAngle(int angle) { fixedClampServo->write(angle); }
  void extrude(float mmOfFilament, float mmPerMinuteFeedrate);
  float calculateExtrusionAmount(float startPosition, float endPosition);
  float calculateEndPosition(float startPosition); 
  void home(float fixedClamp, float movingClamp, float pusher);
 private:
  IServo* pusherServo = nullptr;
  IServo* movingClampServo = nullptr;
  IServo* fixedClampServo = nullptr;
  float clampingDelayMillis;
  float movementDelayMillis;
  float mmToExtrude;
  //int state;
  String state;  // For development
  unsigned long nextActionMillis;

};
