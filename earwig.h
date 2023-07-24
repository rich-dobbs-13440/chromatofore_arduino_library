#pragma once

#include <Arduino.h>

#include "iFilamentDetector.h"
#include "iServo.h"

const int SERVOS_PER_EARWIG_ACTUATOR = 3;

class EarwigFilamentActuator {
 public:
  EarwigFilamentActuator();
  ~EarwigFilamentActuator();
  // Disable copy constructor
  EarwigFilamentActuator(const EarwigFilamentActuator&) = delete;
  // Disable copy assignment operator
  EarwigFilamentActuator& operator=(const EarwigFilamentActuator&) = delete;

  void dump();

  void initialize(IServo& pusherServo, IServo& movingClampServo,
                  IServo& fixedClampServo,
                  IFilamentDetector& filamentDetector) {
    this->pusherServo = &pusherServo;
    this->movingClampServo = &movingClampServo;
    this->fixedClampServo = &fixedClampServo;
    this->filamentDetector = &filamentDetector;
  }

  void begin(int minimumFixedClampServoAngle, int maximumFixedClampServoAngle,
             int minimumMovingClampServoAngle, int maximumMovingClampServoAngle,
             int minumPusherServoAngle, int maximumPusherServoAngle);
  void loop();
  void setPusherServoAngle(int angle) { pusherServo->write(angle); }
  void setMovingClampServoAngle(int angle) { movingClampServo->write(angle); }
  void setFixedClampServoAngle(int angle) { fixedClampServo->write(angle); }
  void extrude(float mmOfFilament, float mmPerMinuteFeedrate, bool use_filament_detector, bool require_filament);
  float calculateExtrusionAmount(float startPosition, float endPosition);
  float calculateEndPosition(float startPosition);
  void home(float fixedClamp, float movingClamp, float pusher);
  void printSwitchStates();
  bool isBusy();

 private:
  IServo* pusherServo = nullptr;
  IServo* movingClampServo = nullptr;
  IServo* fixedClampServo = nullptr;
  IFilamentDetector* filamentDetector = nullptr;
  float clampingDelayMillis;
  float movementDelayMillis;
  float mmToExtrude;
  // int state;
  String state;  // For development
  unsigned long nextActionMillis;
  bool use_filament_detector = false;
  bool require_filament = false;
};
