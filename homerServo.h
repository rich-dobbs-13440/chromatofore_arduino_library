// HomerServo.h

#pragma once

#include "iServo.h"
#include "iSwitch.h"
#include "Arduino.h"

enum class HomingState {
    Idle,    
    Begin,
    FastSweep,
    Retracting,
    FinalSweep,
    SetMinAngle,
    ReportFailure, 
    WaitForServoMovement,
    Failed, 
};

class HomerServo : public IServo {
private:
    HomingState currentState = HomingState::Idle;
    HomingState failedState = HomingState::Idle;
    ISwitch* limitSwitch = nullptr;
    unsigned long stateStartTime;
    unsigned long nextActionTime; 
    int fastSweepInitialAngle = 90;
    int fastSweepAngleIncrement = 2;  
    bool debug = true;
    int minimumAngle = -1;
    int maximumAngle = -1;

protected:
    unsigned int  movementDelayMillis = 1000;
    unsigned int  switchDelayMillis = 500;
    unsigned int  sweepDelayMillis = 50;
    int currentAngle = -1; 

    void dumpState();

public:
    HomerServo();
    void loop() override;
    void home(ISwitch* limitSwitch) override;
    bool isIdle() override {
        return currentState == HomingState::Idle;
    }
    bool isFailed() override {
        return currentState == HomingState::Failed;
    }
    virtual void reset() override {
        failedState = HomingState::Idle;
        currentState = HomingState::Idle;
    }   

    static String homingStateToString(HomingState state);

    void setMinimumAngle(int minimumAngle) override {
        this->minimumAngle = minimumAngle;
    }
    void setMaximumAngle(int maximumAngle) override {
        this->maximumAngle = maximumAngle;
    }
    int getMinimumAngle() override {
        return minimumAngle;
    }
    int getMaximumAngle() override {
        return maximumAngle;
    }; 
    int getCurrentAngle() override {
        return currentAngle;
    };
    ExpectedArrivalMillis position(float relativePosition) override {
        int angle = minimumAngle + relativePosition * (maximumAngle - minimumAngle);
        return write(angle);
    }    

};
