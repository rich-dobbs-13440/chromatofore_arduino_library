#pragma once

#include <PCF8574.h>
#include "iBoard.h"


class PCF8574GPIOMultiplexer : public iBoard {
public:
  PCF8574GPIOMultiplexer(byte i2cAddress)
    : i2cAddress(i2cAddress), pcf8574(i2cAddress) {}
  void initialize() override {
    // // Initialize Wire library and set the pin configuration of the PCF8574
    // Wire.begin();
    // Wire.beginTransmission(i2cAddress);
    // Wire.write(0xFF);  // Set all pins as inputs (high impedance)
    // Wire.endTransmission();


    pcf8574.pinMode(P0, INPUT);
    pcf8574.pinMode(P1, INPUT);
    pcf8574.pinMode(P2, INPUT);
    pcf8574.pinMode(P3, INPUT);
    pcf8574.pinMode(P4, INPUT);
    pcf8574.pinMode(P5, INPUT);
    pcf8574.pinMode(P6, INPUT);
    pcf8574.pinMode(P7, INPUT);

    Serial.print("Init pcf8574...");
    if (pcf8574.begin()) {
      Serial.println("OK");
    } else {
      Serial.println("KO");
    }
  }
  void printConfig() override {
    Serial.print("PCF8574GPIOMultiplexer, I2C address: 0x");
    Serial.println(i2cAddress, HEX);
  }
  bool readPin(byte pinNumber) {
    if (pinNumber >= 0 && pinNumber < 8) {
      // Read the state of all pins from the PCF8574
      Wire.requestFrom(i2cAddress, static_cast<int>(1));
      if (Wire.available()) {
        byte pinStates = Wire.read();

        // Extract the state of the specified pin (0 = LOW, 1 = HIGH)
        return bitRead(pinStates, pinNumber);
      } else {
        Serial.println("Wire.available() is false!");
      }
    }

    // If the pin number is invalid, return false
    return false;
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
private:
  byte i2cAddress;
  PCF8574 pcf8574;
};