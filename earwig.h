#pragma once

#include <Arduino.h>
//#include <Wire.h>
//#include <Adafruit_PWMServoDriver.h>
#include "iServo.h"

class EarwigFilamentActuator {
  public:
    EarwigFilamentActuator(IServo& pusherServo, IServo& movingClampServo, IServo& fixedClampServo); 

    void begin();
    void setPusherServoAngle(int angle) {
      pusherServo.write(angle);
    }        
    void setMovingClampServoAngle(int angle) {
      movingClampServo.write(angle);
    }    
    void setFixedClampServoAngle(int angle) {
      fixedClampServo.write(angle);
    }

    // void extendFilament();    // Method to extend the filament
    // void retractFilament();   // Method to retract the filament
    // bool isLoaded();        // Method to check if the filament is extended
    // void loadFilament();
    // void unloadFilament();
    // void releaseClamps();
  private:
    //bool isFilamentLoaded;  // Flag to indicate the filament status
    IServo& pusherServo;
    IServo& movingClampServo;
    IServo& fixedClampServo;

};
