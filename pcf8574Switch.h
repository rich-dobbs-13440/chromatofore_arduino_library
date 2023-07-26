#pragma once

#include "iSwitch.h"
#include "pcf8574GPIOMultiplexer.h"

struct Pcf8574SwitchInfo {
  int i2cAddress;
  int pin;
  int actuator;
  int role;
};


Pcf8574SwitchInfo getPcf8574SwitchInfo(int numRows, int gpioConfiguration[][4], int actuator, int role);

class Pcf8574Switch : public ISwitch{
  public:
  Pcf8574Switch(Pcf8574SwitchInfo switchInfo, PCF8574GPIOMultiplexer& multiplexer){
    i2cAddress = switchInfo.i2cAddress;
    pin = switchInfo.pin;
    actuator = switchInfo.actuator;
    this->multiplexer = &multiplexer;
  }
  void begin() override {}
  SwitchState read() override {
    multiplexer->readAndPrintPins();

    auto pinState = multiplexer->readPin(pin);

    multiplexer->readAndPrintPins();
    if (pinState == PinState::HIGH) {
      return SwitchState::Triggered;
    } else if (pinState == PinState::LOW) {
      return SwitchState::Untriggered;
    } else {
      return SwitchState::Error;
    }
  };  
    private:
        int i2cAddress;
        int pin;
        int actuator;
        int role;
        PCF8574GPIOMultiplexer* multiplexer = nullptr;

};