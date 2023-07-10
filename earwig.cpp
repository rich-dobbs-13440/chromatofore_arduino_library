#include "earwig.h"

EarwigFilamentActuator::EarwigFilamentActuator(IServo& pusherServo,
                                               IServo& movingClampServo,
                                               IServo& fixedClampServo)
    : pusherServo(pusherServo),
      movingClampServo(movingClampServo),
      fixedClampServo(fixedClampServo) {}

void EarwigFilamentActuator::begin(int minimumFixedClampServoAngle,
                                   int maximumFixedClampServoAngle,
                                   int minimumMovingClampServoAngle,
                                   int maximumMovingClampServoAngle,
                                   int minumPusherServoAngle,
                                   int maximumPusherServoAngle) {
  // Initialize with all servos in minimum position, which corresponds to the 
  // clamps being open, and the moving clamp positioned near the fixed clamp.                                    
  fixedClampServo.begin(minimumFixedClampServoAngle, minimumMovingClampServoAngle, 0);
  movingClampServo.begin(minimumMovingClampServoAngle, maximumFixedClampServoAngle, 0);
  pusherServo.begin(minumPusherServoAngle, maximumPusherServoAngle, 0);
}