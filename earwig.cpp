#include "earwig.h"

const float FRONT_POSITION = 0;
const float BACK_POSITION = 1;
const float OPEN_POSITION = 0;
const float CLOSED_POSITION = 1;

EarwigFilamentActuator::EarwigFilamentActuator(IServo& pusherServo,
                                               IServo& movingClampServo,
                                               IServo& fixedClampServo)
    : pusherServo(pusherServo),
      movingClampServo(movingClampServo),
      fixedClampServo(fixedClampServo) {
  clampingDelayMillis = 3000;
  movementDelayMillis = 3000;
}

void EarwigFilamentActuator::begin(int minimumFixedClampServoAngle,
                                   int maximumFixedClampServoAngle,
                                   int minimumMovingClampServoAngle,
                                   int maximumMovingClampServoAngle,
                                   int minimumPusherServoAngle,
                                   int maximumPusherServoAngle) {
  // Initialize with all servos in minimum position, which corresponds to the
  // clamps being open, and the moving clamp positioned near the fixed clamp.
  fixedClampServo.begin(minimumFixedClampServoAngle,
                        maximumFixedClampServoAngle, 0);
  movingClampServo.begin(minimumMovingClampServoAngle,
                         maximumMovingClampServoAngle, 0);
  pusherServo.begin(minimumPusherServoAngle, maximumPusherServoAngle, 0);
}



void EarwigFilamentActuator::extrude(float mmOfFilament,
                                     float mmPerMinuteFeedrate) {
  //fixedClampServo.dump();
  // For now ignore feedrate and amouont of extrusion.  Just move a chunk.
  float pusherStartMosition = mmOfFilament > 0 ? FRONT_POSITION : BACK_POSITION;
  float pusherEndPosition = mmOfFilament > 0 ? BACK_POSITION : FRONT_POSITION;
  fixedClampServo.position(CLOSED_POSITION);
  delay(clampingDelayMillis);
  fixedClampServo.dump();
  movingClampServo.position(OPEN_POSITION);
  fixedClampServo.position(CLOSED_POSITION);
  delay(clampingDelayMillis);
  movingClampServo.dump();
  pusherServo.position(pusherStartMosition);
  delay(movementDelayMillis);
  pusherServo.dump();
  fixedClampServo.position(OPEN_POSITION);
  movingClampServo.position(CLOSED_POSITION);
  delay(clampingDelayMillis);
  pusherServo.position(pusherEndPosition);
}