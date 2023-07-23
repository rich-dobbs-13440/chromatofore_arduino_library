#pragma once

#include <PCF8574.h>
#include "iBoard.h"
#include "debugLog.h"

enum class PinState {
    HIGH,
    LOW,
    ERROR
};


class PCF8574GPIOMultiplexer : public iBoard {
public:
  PCF8574GPIOMultiplexer(byte i2cAddress)
    : i2cAddress(i2cAddress), pcf8574(i2cAddress) {}
  void initialize() override {

    pcf8574.pinMode(P0, INPUT);
    pcf8574.pinMode(P1, INPUT);
    pcf8574.pinMode(P2, INPUT);
    pcf8574.pinMode(P3, INPUT);
    pcf8574.pinMode(P4, INPUT);
    pcf8574.pinMode(P5, INPUT);
    pcf8574.pinMode(P6, INPUT);
    pcf8574.pinMode(P7, INPUT);

    if (pcf8574.begin()) {
      debugLog("Init pcf8574  OK");
    } else {
      debugLog("Init pcf8574 failed");
    }
  }
  void printConfig() override {
    Serial.print("PCF8574GPIOMultiplexer, I2C address: 0x");
    Serial.println(i2cAddress, HEX);
  }
  PinState readPin(uint8_t pinNumber) {
    Serial.print("I2C: 0x");
    Serial.print(i2cAddress, HEX);
    Serial.println("  ");
    if (pinNumber >= 0 && pinNumber < 8) {
      pcf8574.readBuffer();
      uint8_t val = pcf8574.digitalRead(pinNumber, true);
      debugLog("pinNumber", pinNumber, "val: ", val);
      if (val == HIGH) {
        debugLog("returning PinState::HIGH");
        return PinState::HIGH;
      } else {
        debugLog("returning PinState::LOW");
        return PinState::LOW;
      };
    }

    debugLog("Bad pin number: ", pinNumber);
    return PinState::ERROR;
  }

  void readAndPrintPins() {
    Serial.print("I2C: 0x");
    Serial.print(i2cAddress, HEX);
    Serial.print("  ");
    pcf8574.readBuffer();
    // read value on buffer than reset value for that pin
    uint8_t val = pcf8574.digitalRead(P0);
    if (val == HIGH) {
      Serial.print("P0 HIGH  ");
    } else {
      Serial.print("P0 LOW   ");
    }
    val = pcf8574.digitalRead(P1);
    if (val == HIGH) {
      Serial.print("P1 HIGH  ");
    } else {
      Serial.print("P1 LOW   ");
    }
    val = pcf8574.digitalRead(P2);
    if (val == HIGH) {
      Serial.print("P2 HIGH  ");
    } else {
      Serial.print("P2 LOW   ");
    }
    val = pcf8574.digitalRead(P3);
    if (val == HIGH) {
      Serial.print("P3 HIGH  ");
    } else {
      Serial.print("P3 LOW   ");
    }
    val = pcf8574.digitalRead(P4);
    if (val == HIGH) {
      Serial.print("P4 HIGH  ");
    } else {
      Serial.print("P4 LOW   ");
    }
    val = pcf8574.digitalRead(P5);
    if (val == HIGH) {
      Serial.print("P5 HIGH  ");
    } else {
      Serial.print("P5 LOW   ");
    }
    val = pcf8574.digitalRead(P6);
    if (val == HIGH) {
      Serial.print("P6 HIGH  ");
    } else {
      Serial.print("P6 LOW   ");
    }
    val = pcf8574.digitalRead(P7);
    if (val == HIGH) {
      Serial.print("P7 HIGH  ");
    } else {
      Serial.print("P7 LOW   ");
    }
    Serial.println();

  }

  void pcf8574Loop(int seconds) {
    int intervalSeconds = 2;
    int loopCount = seconds / intervalSeconds;
    for (int loop = 0; loop < loopCount; loop++) {
      readAndPrintPins();
      delay(1000 * intervalSeconds);
    }
  }
  byte getI2CAddress() const {
    return i2cAddress;

  }
private:
  byte i2cAddress;
  PCF8574 pcf8574;
};