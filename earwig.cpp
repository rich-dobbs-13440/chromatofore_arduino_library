#include "earwig.h"

#include "debugLog.h"

const float FRONT_POSITION = 0;
const float BACK_POSITION = 1;
const float OPEN_POSITION = 0;
const float CLOSED_POSITION = 1;


float mmPerStep = 24.0;

EarwigFilamentActuator::EarwigFilamentActuator() {

  mmToExtrude = 0.0;

  currentState = EarwigState::Idle;  
  nextActionMillis = -1;
}

EarwigFilamentActuator::~EarwigFilamentActuator() {
  if (filamentDetector != nullptr) {
    delete filamentDetector;
    filamentDetector = nullptr;
  }
  if (movingClampLimitSwitch != nullptr) {
    delete movingClampLimitSwitch;
    movingClampLimitSwitch = nullptr;
  }
}

void EarwigFilamentActuator::dump() {
  Serial.print("Address:");
  Serial.println((uintptr_t)this, HEX);
  //debugLog("clampingDelayMillis:", clampingDelayMillis);
  // debugLog("movementDelayMillis:", movementDelayMillis);
  // debugLog("sweepDelayMillis:", sweepDelayMillis);
  // debugLog("switchDelayMillis:", switchDelayMillis);
  
  debugLog("mmToExtrude", mmToExtrude, "currentState:", earwigStateToString(currentState), "nextActionMillis:", nextActionMillis);
}

bool EarwigFilamentActuator::isBusy() {
  return currentState != EarwigState::Idle; 
}

void EarwigFilamentActuator::begin(int minimumFixedClampServoAngle,
                                   int maximumFixedClampServoAngle,
                                   int minimumMovingClampServoAngle,
                                   int maximumMovingClampServoAngle,
                                   int minimumPusherServoAngle,
                                   int maximumPusherServoAngle) {
  // Initialize with all servos in minimum position, which corresponds to the
  // clamps being open, and the moving clamp positioned near the fixed clamp.
  fixedClampServo->begin(minimumFixedClampServoAngle,
                         maximumFixedClampServoAngle, 0);
  movingClampServo->begin(minimumMovingClampServoAngle,
                          maximumMovingClampServoAngle, 0);
  pusherServo->begin(minimumPusherServoAngle, maximumPusherServoAngle, 0);

  currentState = EarwigState::Idle;

  // dump();
}

void end_extrusion() {

}

void EarwigFilamentActuator::loop() {
    auto startingState = currentState;
    auto startingNextActionMillis = nextActionMillis;
    if (currentState == EarwigState::Idle) {
        return;
    }
    if (currentState == EarwigState::Homing){
        pusherServo->loop();
        fixedClampServo->loop();
        movingClampServo->loop();
        if (pusherServo->isIdle() && fixedClampServo->isIdle() && movingClampServo->isIdle()) {
          nextActionMillis = millis();
          currentState = EarwigState::Idle;
          return;
        }
    }
    if (millis() > nextActionMillis) {
        // debugLog("Starting state: ", earwigStateToString(currentState), "   mmToExtrude:", mmToExtrude);
        if (currentState == EarwigState::LockToStart) {
            auto expectedArrivalTimeFCS = fixedClampServo->position(CLOSED_POSITION);
            auto expectedArrivalTimeMCS = movingClampServo->position(OPEN_POSITION);
            nextActionMillis = max(expectedArrivalTimeFCS, expectedArrivalTimeMCS);
            currentState = EarwigState::MoveToStart;
        } else if (currentState == EarwigState::MoveToStart) {

            float startPosition = mmToExtrude > 0 ? FRONT_POSITION : BACK_POSITION;
            auto expectedArrivalTime = pusherServo->position(startPosition);
            nextActionMillis = expectedArrivalTime;
            currentState = EarwigState::LockForExtrude;
        } else if (currentState == EarwigState::LockForExtrude) {
            auto expectedArrivalTimeFCS = fixedClampServo->position(OPEN_POSITION);
            auto expectedArrivalTimeMCS = movingClampServo->position(CLOSED_POSITION);
            nextActionMillis = max(expectedArrivalTimeFCS, expectedArrivalTimeMCS);
            currentState = EarwigState::MoveFilament;
        } else if (currentState == EarwigState::MoveFilament) {
            bool stop_moving = false;
            if (use_filament_detector) {
                auto filamentState = filamentDetector->read();
                if (filamentState == FilamentDetectorState::Undetected) {
                    stop_moving = require_filament;
                    debugLog("FilamentDetectorState::Undetected.  stop_moving:", stop_moving);
                } else if (filamentState == FilamentDetectorState::Detected) {
                    stop_moving = !require_filament;
                    debugLog("FilamentDetectorState::Detected.  stop_moving:", stop_moving);
                } else {
                    stop_moving = true;
                    debugLog("FilamentDetectorState::Error.  stop_moving:", stop_moving);
                }
            }
            if (!stop_moving) {
                float startPosition = mmToExtrude > 0 ? FRONT_POSITION : BACK_POSITION;
                float endPosition = calculateEndPosition(startPosition);
                auto expectedArrivalTime = pusherServo->position(endPosition);
                mmToExtrude -= calculateExtrusionAmount(startPosition, endPosition);
                nextActionMillis = expectedArrivalTime;
            }
            if (!stop_moving && abs(mmToExtrude) > 1) {
                currentState = EarwigState::LockToStart;
            } else {
                mmToExtrude = 0;
                auto expectedArrivalTimeFCS = fixedClampServo->position(OPEN_POSITION);
                auto expectedArrivalTimeMCS = movingClampServo->position(CLOSED_POSITION);
                auto expectedArrivalTime = max(expectedArrivalTimeFCS, expectedArrivalTimeMCS);
                delay(expectedArrivalTime - millis());
                fixedClampServo->atEase();
                movingClampServo->atEase();
                pusherServo->atEase();
                currentState = EarwigState::Idle;
            }
        }
        if (currentState != startingState || nextActionMillis != startingNextActionMillis) {
            //dumpState();
            debugLog("Ending state: '", earwigStateToString(currentState), "' nextActionMillis:", nextActionMillis, "mmToExtrude:", mmToExtrude);
        }
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
                                     float mmPerMinuteFeedrate, 
                                    bool use_filament_detector, 
                                    bool require_filament) {
  this->use_filament_detector = use_filament_detector;
  this->require_filament = require_filament;
  mmToExtrude += mmOfFilament;
  debugLog("Current state: '", earwigStateToString(currentState), "' mmToExtrude:", mmToExtrude);
  // Ignore feedrate for now.

  if (currentState == EarwigState::Idle) {
    currentState = EarwigState::LockToStart;  //LOCK_TO_START_STATE;
    debugLog("Entering state: '", earwigStateToString(currentState), "' mmToExtrude:", mmToExtrude);
    nextActionMillis = millis();
  }
}

void EarwigFilamentActuator::home_pusher_and_update_servo_min_angle() {
  // Get the clamps open and at ease, so that the pusher can move freely.
  debugLog("In home_pusher_and_update_servo_min_angle");
  auto expectedArrivalTimeFCS = fixedClampServo->position(OPEN_POSITION);
  auto expectedArrivalTimeMCS = movingClampServo->position(OPEN_POSITION);
  auto expectedArrivalTime = max(expectedArrivalTimeFCS, expectedArrivalTimeMCS);
  delay(expectedArrivalTime - millis());
  fixedClampServo->atEase();
  movingClampServo->atEase();  
  pusherServo->home(movingClampLimitSwitch);
}


void EarwigFilamentActuator::home(float fixedClamp, float movingClamp,
                                  float pusher) {
  // The arguments are either nan or a value.  The exact value is ignored.
  if (isnan(fixedClamp) && isnan(movingClamp) && isnan(pusher)) {
    // Home all axes if no additional parameters given
    fixedClampServo->home(nullptr);
    movingClampServo->home(nullptr);
    home_pusher_and_update_servo_min_angle();
  } else {
    if (!isnan(fixedClamp)) {
      fixedClampServo->home(nullptr);
    }
    if (!isnan(movingClamp)) {
      movingClampServo->home(nullptr);
    }
    if (!isnan(pusher)) {
      home_pusher_and_update_servo_min_angle();
    }
  }
  currentState = EarwigState::Homing;
  nextActionMillis = millis();  
}

void EarwigFilamentActuator::printSwitchStates() {
  debugLog("printSwitchStates:");
  if (filamentDetector) {
    auto state = filamentDetector->read();
    if (state == FilamentDetectorState::Detected) {
      debugLog("Filament detected!");
    } else if (state == FilamentDetectorState::Undetected) {
      debugLog("Filament not detected!");
    } else {
      debugLog("Error in printSwitchStates", int(state));
    }
  } else {
    debugLog("No filament detector in Earwig.");
  }
  if (movingClampLimitSwitch) {
    auto state = movingClampLimitSwitch->read();
    if (state == SwitchState::Triggered) {
      debugLog("Limit Switch Triggered!");
    } else if (state == SwitchState::Untriggered) {
      debugLog("Limit switch not Triggered");
    } else {
      debugLog("Error in printSwitchStates", int(state));
    }
  } else {
    debugLog("No limit_switch in Earwig.");
  }
}


String EarwigFilamentActuator::earwigStateToString(EarwigState state) {
    switch (state) {
        case EarwigState::Idle:
            return "Idle State";
        case EarwigState::LockToStart:
            return "Lock To Start State";
        case EarwigState::MoveToStart:
            return "Move To Start State";
        case EarwigState::LockForExtrude:
            return "Lock For Extrude State";
        case EarwigState::MoveFilament:
            return "Move Filament State";
        case EarwigState::Homing:
            return "Homing State";
        default:
            return "Unknown State";
    }
}

