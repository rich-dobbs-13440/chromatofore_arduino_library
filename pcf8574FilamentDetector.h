#pragma once

#include "iFilamentDetector.h"

class Pcf8574FilamentDetector : public IFilamentDetector {
 private:
  int actuator = -1;
  int pin = -1;
  PCF8574GPIOMultiplexer* multiplexer = nullptr;

 public:
  Pcf8574FilamentDetector() {}

  void initialize( Pcf8574SwitchInfo filamentDetectorSwitchInfo, PCF8574GPIOMultiplexer& multiplexer) {
      actuator = filamentDetectorSwitchInfo.actuator;
      pin = filamentDetectorSwitchInfo.pin;
      this->multiplexer = &multiplexer;
  }


  void begin() override {
  };

  FilamentDetectorState read() override {
    bool pinState = multiplexer->readPin(pin);
    if (pinState == true) {
      return FilamentDetectorState::Detected;
    } else {
      return FilamentDetectorState::Undetected;
    }
  };
};