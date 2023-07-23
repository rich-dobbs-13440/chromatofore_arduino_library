#pragma once

#include "iFilamentDetector.h"

class Pcf8574FilamentDetector : public IFilamentDetector {
 private:
  String id;
  PCF8574GPIOMultiplexer* multiplexer;
  int pin = -1;

 public:
  Pcf8574FilamentDetector(Pcf8574SwitchInfo filamentDetectorSwitchInfo) {}

  void initialize(PCF8574GPIOMultiplexer*  multiplexer) {
    this->multiplexer = multiplexer;
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