#pragma once

#include <Arduino.h>
#include "iServo.h"
#include "iFilamentDetector.h"
#include "pcf8574Switch.h"
#include "pcf8574FilamentDetector.h"

const int SERVOS_PER_EARWIG_ACTUATOR = 3;

class EarwigFilamentActuator {
 public:
  EarwigFilamentActuator();
  // Disable copy constructor
  EarwigFilamentActuator(const EarwigFilamentActuator&) = delete; 
  // Disable copy assignment operator
  EarwigFilamentActuator& operator=(const EarwigFilamentActuator&) = delete; 
   

  void dump();
  
  void initialize(IServo& pusherServo, IServo& movingClampServo, IServo& fixedClampServo, Pcf8574SwitchInfo filamentDetectorSwitchInfo) {
                          this->pusherServo = &pusherServo;
                          this->movingClampServo = &movingClampServo;
                          this->fixedClampServo = &fixedClampServo;
                          this->filamentDetector =  new Pcf8574FilamentDetector(filamentDetectorSwitchInfo);

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
  void printSwitchStates();
 private:
  IServo* pusherServo = nullptr;
  IServo* movingClampServo = nullptr;
  IServo* fixedClampServo = nullptr;
  IFilamentDetector* filamentDetector = nullptr;
  float clampingDelayMillis;
  float movementDelayMillis;
  float mmToExtrude;
  //int state;
  String state;  // For development
  unsigned long nextActionMillis;

};
