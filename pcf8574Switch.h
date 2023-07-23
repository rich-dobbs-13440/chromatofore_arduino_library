#pragma once


#include "pcf8574GPIOMultiplexer.h"

struct Pcf8574SwitchInfo {
  int i2cAddress;
  int pin;
  int actuator;
  int role;
};


Pcf8574SwitchInfo getPcf8574SwitchInfo(int gpioConfiguration[][4], int numRows, int actuator, int role);

class Pcf8574Switch{
    private:
        int i2cAddress;
        int pin;
        int actuator;
        int role;
        PCF8574GPIOMultiplexer* multiplexer = nullptr;

};