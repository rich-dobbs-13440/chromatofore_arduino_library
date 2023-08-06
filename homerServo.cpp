// HomerServo.cpp

#include "homerServo.h"
#include "debugLog.h"

HomerServo::HomerServo() {
    currentState = HomingState::Idle;
    stateStartTime = 0;
    nextActionTime = 0;
    

    currentAngle = -1;
    int minimumAngle = -1;
    int maximumAngle = -1;
    int currentAngle = -1;

}

void HomerServo::dumpState() {
    if (debug) {
        debugLog("State", homingStateToString(currentState), "stateStartTime", stateStartTime, "nextActionTime", nextActionTime, "angle", currentAngle);
    }
}

void HomerServo::home(ISwitch* limitSwitch) {
    this->limitSwitch = limitSwitch;
    this->currentState = HomingState::Begin;
    this->stateStartTime = millis();
    this->nextActionTime = this->stateStartTime;  
    dumpState();
}

// HomerServo.cpp
void HomerServo::loop() {
    auto startingState = currentState;
    unsigned long startingNextActionTime = nextActionTime;
    unsigned long currentTime = millis();
    switch(currentState) {
        case HomingState::Idle:
            break;
        case HomingState::Begin:
            if (currentTime >= nextActionTime) {
                if (limitSwitch) {
                    // Tell servo to move to initial angle to start sweep
                    auto expectedTime = write(fastSweepInitialAngle);
                    currentState = HomingState::FastSweep;
                    stateStartTime = currentTime;
                    nextActionTime = expectedTime + switchDelayMillis;
                } else {
                    // Tell servo to move to default start position
                    position(0);
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
                    // Tell servo to back away some.
                    auto expectedTime = write(currentAngle+=10);
                    stateStartTime = currentTime;
                    nextActionTime = expectedTime + switchDelayMillis;
                } else {
                    int angle = currentAngle - fastSweepAngleIncrement;
                    if (angle < 0) {
                        failedState = currentState;
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
                int angle = currentAngle + 5;
                if (angle > fastSweepInitialAngle) {
                    failedState = currentState;
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
            break;

        case HomingState::FinalSweep:
            if (currentTime >= nextActionTime) {
                if (limitSwitch->Triggered()) {
                    currentState = HomingState::SetMinAngle;
                    stateStartTime = currentTime;
                    nextActionTime = currentTime;
                } else {
                    int angle = currentAngle - 1;
                    if (angle < 0) {
                        failedState = currentState;
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

        case HomingState::SetMinAngle:
            {
                setMinimumAngle(currentAngle);
                debugLog("Minumum angle is now:", currentAngle);
                currentState = HomingState::Idle;
                auto expectedArrivalTime = position(0);
                nextActionTime = expectedArrivalTime;
            }
            break;

        
        case HomingState::ReportFailure: 
            {    
                debugLog("FAILURE:  Homing Failed.  State that failed:", homingStateToString(failedState), "currentAngle", currentAngle);
                debugLog("   Is switch malfunctioning?");
                debugLog("   Is servo horn installed properly?");
                debugLog("   Is moving clamp binding in slide?");  
                debugLog("   Are other servos in appropriate position for homing?"); 
                currentState = HomingState::Failed;              
                nextActionTime = currentTime;    
            } 
            break;

        case HomingState::Failed:
            // Will stay in failed state until reset;
            break;


        default:
            // Handle unknown states, possibly reset to a known state
            debugLog("Unhandled state: ", homingStateToString(currentState));
            break;
    }
    if (currentState != startingState || nextActionTime != startingNextActionTime) {
        dumpState();
    }
    
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
            return "Report Failure State";
        case HomingState::Failed:
            return "Failed State";
        case HomingState::WaitForServoMovement:
            return "Wait For Servo Movement State";
        default:
            return "Unknown State";
    }
}
