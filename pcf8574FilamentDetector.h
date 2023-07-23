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
    
    multiplexer->readAndPrintPins();

    auto pinState = multiplexer->readPin(pin);

    multiplexer->readAndPrintPins();
    if (pinState == PinState::HIGH) {
      return FilamentDetectorState::Detected;
    } else if (pinState == PinState::LOW) {
      return FilamentDetectorState::Undetected;
    } else {
      return FilamentDetectorState::Error;
    }
  };
};