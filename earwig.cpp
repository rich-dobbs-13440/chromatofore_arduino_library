#include "earwig.h"

#include "debugLog.h"

const float FRONT_POSITION = 0;
const float BACK_POSITION = 1;
const float OPEN_POSITION = 0;
const float CLOSED_POSITION = 1;

// const int IDLE_STATE = 0;
// const int LOCK_TO_START_STATE  = 1;
// const int MOVE_TO_START_STATE = 2;
// const int LOCK_FOR_EXTRUDE_STATE  = 3;
// const int MOVE_FILAMENT_STATE = 4;

const String IDLE_STATE = "IDLE";
const String LOCK_TO_START_STATE  = "LOCK_TO_START";
const String MOVE_TO_START_STATE  = "MOVE_TO_START";
const String LOCK_FOR_EXTRUDE_STATE  = "LOCK_FOR_EXTRUDE";
const String MOVE_FILAMENT_STATE = "MOVE_FILAMENT";

float mmPerStep = 24.0;

EarwigFilamentActuator::EarwigFilamentActuator(IServo& pusherServo,
                                               IServo& movingClampServo,
                                               IServo& fixedClampServo)
    : pusherServo(pusherServo),
      movingClampServo(movingClampServo),
      fixedClampServo(fixedClampServo) {
  clampingDelayMillis = 3000;
  movementDelayMillis = 3000;
  mmToExtrude = 0.0;

  state = IDLE_STATE;
  nextActionMillis = -1;
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

void EarwigFilamentActuator::loop() {
  if (state == IDLE_STATE) {
    return;
  }
  if (millis() > nextActionMillis) {
    debugLog("Starting state: ", state, "mmToExtrude:", mmToExtrude);
    if (state == LOCK_TO_START_STATE) {
      fixedClampServo.position(CLOSED_POSITION);
      movingClampServo.position(OPEN_POSITION);
      nextActionMillis = millis() + clampingDelayMillis;
      state = MOVE_TO_START_STATE;
    } else if (state == MOVE_TO_START_STATE) {
      float startPosition = mmToExtrude > 0 ? FRONT_POSITION : BACK_POSITION;
      pusherServo.position(startPosition);
      nextActionMillis = millis() + movementDelayMillis;
      state = LOCK_FOR_EXTRUDE_STATE;
    } else if (state == LOCK_FOR_EXTRUDE_STATE) {
      fixedClampServo.position(OPEN_POSITION);
      movingClampServo.position(CLOSED_POSITION);
      nextActionMillis = millis() + clampingDelayMillis;
      state = MOVE_FILAMENT_STATE;
    } else if (state == MOVE_FILAMENT_STATE) {
      float startPosition = mmToExtrude > 0 ? FRONT_POSITION : BACK_POSITION;
      float endPosition = calculateEndPosition(startPosition);
      pusherServo.position(endPosition);
      mmToExtrude -= calculateExtrusionAmount(startPosition, endPosition);
      nextActionMillis = millis() + movementDelayMillis;
      if (abs(mmToExtrude) > 1) {
        state = LOCK_TO_START_STATE;
      } else {
        mmToExtrude == 0;
        state = IDLE_STATE;
      }
    }
    debugLog("Ending state: ", state, "nextActionMillis:", nextActionMillis, "mmToExtrude:", mmToExtrude);
  }
}

float EarwigFilamentActuator::calculateExtrusionAmount(float startPosition,
                                                       float endPosition) {
  return (endPosition - startPosition) * mmPerStep;
}

float EarwigFilamentActuator::calculateEndPosition(float startPosition) {
  // TODO, use actual kinematics

  if (mmToExtrude > 0) {
    return min(mmToExtrude / mmPerStep, 1);
  } else {
    return max(mmToExtrude / mmPerStep, 0);
  }
}

void EarwigFilamentActuator::extrude(float mmOfFilament,
                                     float mmPerMinuteFeedrate) {
                                      
  mmToExtrude += mmOfFilament;
  debugLog("Ending state: ", state, "mmToExtrude:", mmToExtrude);
  // Ignore feedrate for now.

  if (state == IDLE_STATE) {
    state = LOCK_TO_START_STATE;
    nextActionMillis = millis();
  }
}

void EarwigFilamentActuator::home(float fixedClamp, float movingClamp, float pusher) {
  // The arguments are either nan or a value.  The exact value is ignored.  
  if (isnan(fixedClamp) && isnan(movingClamp) && isnan(pusher)) {
    // Home all axes if no additional parameters given
    fixedClampServo.position(OPEN_POSITION);
    movingClampServo.position(OPEN_POSITION);
    pusherServo.position(FRONT_POSITION);    
  } else {
    if (!isnan(fixedClamp)) {
      fixedClampServo.position(OPEN_POSITION);
    }
    if (!isnan(movingClamp)) {
      movingClampServo.position(OPEN_POSITION);
    }
    if (!isnan(pusher)) {
      pusherServo.position(FRONT_POSITION);
    }
  }

}

