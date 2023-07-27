// HomerServo.cpp

#include "homerServo.h"
#include "debugLog.h"

HomerServo::HomerServo() {
    currentState = HomingState::Idle;
    stateStartTime = 0;
    nextActionTime = 0;
    limitSwitch = nullptr;

    angle = -1;
    fastSweepInitialAngle = 90;
    fast_sweep_increment = 2;

    movementDelayMillis = 1000;
    switchDelayMillis = 500;
    sweepDelayMillis = 100;
}

// HomerServo.cpp
void HomerServo::loop() {
    unsigned long currentTime = millis();
    switch(currentState) {
        case HomingState::Idle:
            break;
        case HomingState::Begin:
            if (currentTime >= nextActionTime) {
                if (limitSwitch) {
                    angle = fastSweepInitialAngle;
                    // Tell servo to move to initial angle to start sweep
                    auto expectedTime = write(angle);
                    currentState = HomingState::FastSweep;
                    stateStartTime = currentTime;
                    nextActionTime = expectedTime + switchDelayMillis;
                } else {
                    // Tell servo to move to default start position
                    this->position(0);
                    currentState = HomingState::WaitForServoMovement;
                    stateStartTime = currentTime;
                    nextActionTime = currentTime + movementDelayMillis + switchDelayMillis;
                }
            }
            break;

        case HomingState::WaitForServoMovement:
            if (currentTime >= nextActionTime) {
                currentState = HomingState::Idle;
                stateStartTime = currentTime;
                nextActionTime = currentTime;
            }
            break;


        case HomingState::FastSweep:
            if (currentTime >= nextActionTime) {
                if (limitSwitch->Triggered()) {
                    currentState = HomingState::Retracting;
                    angle += 10;
                    // Tell servo to back away some.
                    auto expectedTime = write(angle);
                    stateStartTime = currentTime;
                    nextActionTime = expectedTime + switchDelayMillis;
                } else {
                    angle -= fast_sweep_increment;
                    if (angle < 0) {
                        currentState = HomingState::ReportFailure;
                        nextActionTime = currentTime;

                    } else {
                        // Move the servo a step in the sweep
                        auto expectedTime = write(angle);
                        // For now, expectedTime will be too large, so just increment the current time.
                        nextActionTime = currentTime + sweepDelayMillis;
                    }
                }
            }
            break;

        case HomingState::Retracting:
            if (limitSwitch->Triggered()) {
                // Need to continue backing away if possible
                angle += 5;
                if (angle > fastSweepInitialAngle) {
                    currentState = HomingState::ReportFailure;
                    stateStartTime = currentTime;
                    nextActionTime = currentTime;
                } else {
                    auto expectedTime = write(angle);
                    nextActionTime = expectedTime + switchDelayMillis;
                }
            } else {
                currentState = HomingState::FinalSweep;
                stateStartTime = currentTime;
                nextActionTime = currentTime;
            }

        case HomingState::FinalSweep:
            if (currentTime >= nextActionTime) {
                if (limitSwitch->Triggered()) {
                    currentState = HomingState::SetMinAngle;
                    stateStartTime = currentTime;
                    nextActionTime = currentTime;
                } else {
                    angle -= 1;
                    if (angle < 0) {
                        currentState = HomingState::ReportFailure;
                        nextActionTime = currentTime;
                    } else {
                        // Move the servo a step in the sweep
                        auto expectedTime = write(angle);
                        // For now, expectedTime will be too large, so just increment the current time.
                        nextActionTime = currentTime + sweepDelayMillis;
                    }
                }
            }
            break;

        // Similarly, implement the other states...
        
        case HomingState::ReportFailure:
            // Handle failure, print debug information, etc.
            break;

        default:
            // Handle unknown states, possibly reset to a known state
            debugLog("Unhandled state: ", homingStateToString(currentState));
            break;
    }
}

/* bool EarwigFilamentActuator::home_pusher_and_update_servo_min_angle() {
  // Get the clamps open and at ease, so that the pusher can move freely.

  fixedClampServo->atEase();
  movingClampServo->atEase();  
  // Get the pusher servo into its start position to begin the sweep.
  int angle = 90;
  pusherServo->write(angle);
  delay(movementDelayMillis + switchDelayMillis);
  if (movingClampLimitSwitch->read()  != SwitchState::Triggered) {
    while (movingClampLimitSwitch->read()  != SwitchState::Untriggered) {
      // Fast sweep servo until limit switch is triggered
      angle -= 1;
      pusherServo->write(angle);
      delay(pusherSweepDelayMillis);
      if (angle < 0) {
        debugLog("Failure: At minimum angle of zero, and limit switch not triggered.");
        debugLog("   Is switch malfunctioning?");
        debugLog("   Is servo horn installed properly?");
        debugLog("   Is moving clamp binding?");
        return false;
      }
    }
    // Now back off a bit.
    int first_trigger = angle;
    while(movingClampLimitSwitch->read()  != SwitchState::Untriggered) {
      angle += 5;
      pusherServo->write(angle);
      delay(movementDelayMillis + switchDelayMillis);
      if (angle > first_trigger + 20) {
        debugLog("Failure: Moving away from limit switch and it is not released.");
        debugLog("   Is switch malfunctioning?");
        debugLog("   Is servo horn installed properly?");
        debugLog("   Is moving clamp binding?");
        return false;        
      }
    }
    // Now sweep forward again until limit switch is retriggered.
    while (movingClampLimitSwitch->read()  != SwitchState::Untriggered) {
      // Fast sweep servo until limit switch is triggered
      angle -= 1;
      if (angle < 0) {
        debugLog("Failure: At minimum angle of zero, and limit switch not triggered.");
        debugLog("   Is switch malfunctioning?");
        debugLog("   Is servo horn installed properly?");
        debugLog("   Is moving clamp binding?");
        return false;
      }
      pusherServo->write(angle);
      delay(pusherSweepDelayMillis);         
    }
    pusherServo->setMinimumAngle(angle);
    debugLog("Minumum angle is now:", angle);
    return true;
  } else {
        debugLog("Failure: At starting andle and limit switch is triggered");
        debugLog("   Is switch malfunctioning?");
        debugLog("   Is servo horn installed properly?");
        debugLog("   Is moving clamp binding?");
        return false;
  }
  return true; */


void HomerServo::home(ISwitch* limitSwitch) {
    this->limitSwitch = limitSwitch;
    this->currentState = HomingState::Begin;
    this->stateStartTime = millis();
    this->nextActionTime = this->stateStartTime;  
}


String HomerServo::homingStateToString(HomingState state) {
    switch (state) {
        case HomingState::Idle:
            return "Idle State";
        case HomingState::Begin:
            return "Begin State";
        case HomingState::FastSweep:
            return "Fast Sweep State";
        case HomingState::Retracting:
            return "Retract State";
        case HomingState::FinalSweep:
            return "Final Sweep State";
        case HomingState::SetMinAngle:
            return "Set Mininum Angle State";
        case HomingState::ReportFailure:
            return "Failure State";
        case HomingState::WaitForServoMovement:
            return "Wait For Servo Movement State";
        default:
            return "Unknown State";
    }
}
