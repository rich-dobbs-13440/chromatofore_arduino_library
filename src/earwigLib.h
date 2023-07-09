#ifndef EARWIG_LIB_H
#define EARWIG_LIB_H

#include <Arduino.h>
//#include <Wire.h>
//#include <Adafruit_PWMServoDriver.h>
#include "iServo.h"

class EarwigFilamentActuator {
  public:
    EarwigFilamentActuator(); 
    void connectPusherServo(IServo& servo) {
      pusherServo = &servo;
    }
    void connectMovingClampServo(IServo& servo){
      movingClampServo = &servo;
    }
    void connectFixedClampServo(IServo& servo) {
      fixedClampServo = &servo;
    }
    void setup();
    void extendFilament();    // Method to extend the filament
    void retractFilament();   // Method to retract the filament
    bool isLoaded();        // Method to check if the filament is extended
    void loadFilament();
    void unloadFilament();
    void releaseClamps();
  private:
    bool isFilamentELoaded;  // Flag to indicate the filament status
    IServo* pusherServo = nullptr;
    IServo* movingClampServo = nullptr;
    IServo* fixedClampServo = nullptr;

};

#endif