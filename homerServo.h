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
};

class HomerServo : public IServo {
private:
    HomingState currentState;
    ISwitch* limitSwitch = nullptr;
    unsigned long stateStartTime;
    unsigned long nextActionTime; 
    int angle;  
    int fastSweepInitialAngle;
    int fast_sweep_increment;

protected:
    unsigned int  movementDelayMillis;
    unsigned int  switchDelayMillis;
    unsigned int  sweepDelayMillis;

public:
    HomerServo();
    void loop() override;
    void home(ISwitch* limitSwitch) override;
    static String homingStateToString(HomingState state);
    // Other methods from IServo interface remain pure virtual
    // void begin(int minimumAngle, int maximumAngle, float initialRelativePosition) override = 0;
    // void write(int angle) override = 0;
    // void position(float relativePosition) override = 0;
    // void atEase() override = 0;
    // void detach() override = 0;
    // void dump() override = 0;
    // void setMinimumAngle(int minimumAngle) override = 0;
    // void setMaximumAngle(int maximumAngle) override = 0;
};
