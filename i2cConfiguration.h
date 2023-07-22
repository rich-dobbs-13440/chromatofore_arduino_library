
#pragma once

#include <Wire.h>

#include "iBoard.h"
#include "pca9685ServoDriver.h"
#include "pcf8574GPIOMultiplexer.h"
#include "debugLog.h"

class I2CConfiguration {
 public:
  I2CConfiguration() {
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < MAX_DEVICES_PER_BOARD_TYPE; j++) {
        boards[i][j] = nullptr;
      }
      boardCount[i] = 0;
    }
    delay(2000);
  }

  bool addBoard(BoardType type, byte i2cAddress) {
    if (type == BoardType::PCA9685_ServoDriver) {
      if (boardCount[BoardType::PCA9685_ServoDriver] <
          MAX_DEVICES_PER_BOARD_TYPE) {
        PCA9685ServoDriver* pBoard = new PCA9685ServoDriver(i2cAddress);
        pBoard->initialize();
        boards[BoardType::PCA9685_ServoDriver]
              [boardCount[BoardType::PCA9685_ServoDriver]] = pBoard;
        boardCount[BoardType::PCA9685_ServoDriver]++;
        return true;
      } else {
        Serial.println(
            "Too many PCA9685 Servo Drivers, can't add another one.");
        return false;
      }
    } else if (type == BoardType::PCF8574_GPIO_Multiplexer) {
      if (boardCount[BoardType::PCF8574_GPIO_Multiplexer] <
          MAX_DEVICES_PER_BOARD_TYPE) {
        PCF8574GPIOMultiplexer* pBoard = new PCF8574GPIOMultiplexer(i2cAddress);
        pBoard->initialize();
        boards[BoardType::PCF8574_GPIO_Multiplexer]
              [boardCount[BoardType::PCF8574_GPIO_Multiplexer]] = pBoard;
        boardCount[BoardType::PCF8574_GPIO_Multiplexer]++;
        return true;
      } else {
        Serial.println(
            "Too many PCF8574 GPIO_Multiplexers, can't add another one.");
        return false;
      }
    }
    Serial.print("Unknown board type: ");
    Serial.println(type);
  }

  void printConfiguration() {
    Serial.println("I2C bus configuration:");
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < boardCount[i]; j++) {
        Serial.print("Board: [");
        Serial.print(i);
        Serial.print("][");
        Serial.print(j);
        Serial.print("]: pointer: ");

        Serial.print(reinterpret_cast<int>(boards[i][j]), HEX);
        Serial.print(" ");
        boards[i][j]->printConfig();
      }
    }
  }

  void reset() {
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < boardCount[i]; j++) {
        delete boards[i][j];
      }
      boardCount[i] = 0;
    }
  }

  void setScanned() { scanned = true; }

  bool isScanned() { return scanned; }

  PCF8574GPIOMultiplexer* getGPIOMultiplexer(int index) {
    if (index < boardCount[BoardType::PCF8574_GPIO_Multiplexer]) {
      return static_cast<PCF8574GPIOMultiplexer*>(
          boards[BoardType::PCF8574_GPIO_Multiplexer][index]);
    } else {
      return nullptr;
    }
  }

  PCA9685ServoDriver* getPCA9685ServoDriver(int index) {
    if (index < boardCount[BoardType::PCA9685_ServoDriver]) {
      return static_cast<PCA9685ServoDriver*>(
          boards[BoardType::PCA9685_ServoDriver][index]);
    } else {
      return nullptr;
    }
  }

   PCA9685ServoDriver* getPca9685ServoDriverFromAddress(int i2cAddress) {
    for (int index = 0; index < MAX_DEVICES_PER_BOARD_TYPE; index++) {
      auto driver = static_cast<PCA9685ServoDriver*>(
          boards[BoardType::PCA9685_ServoDriver][index]);
      if (driver->getI2cAddress() == i2cAddress) {
        return driver;
      }
    }
    return nullptr;
   } 

  void scan() {
    reset();
    byte foundDevices = 0;
    // Scan for PCA9685 (0x40 to 0x7F)
    for (byte address = 0x40; address <= 0x7F; address++) {
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("\nPCA9685 servo driver board found at address 0x");
        Serial.print(address, HEX);
        Serial.println();
        addBoard(PCA9685_ServoDriver, address);
        foundDevices++;
      } else {
        Serial.print(".");
      }
    }

    // Scan for PCF8574 (0x20 to 0x27)
    for (byte address = 0x20; address <= 0x27; address++) {
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("\nPCF8574 GPIO multiplexer found at address 0x");
        Serial.print(address, HEX);
        Serial.println();
        addBoard(PCF8574_GPIO_Multiplexer, address);
        foundDevices++;
      } else {
        Serial.print(".");
      }
    }

    if (foundDevices == 0) {
      Serial.println("No devices found");
    } else {
      Serial.print("Found ");
      Serial.print(foundDevices);
      Serial.println(" device(s) on the I2C bus");
    }
    setScanned();
  }



  void begin() {
    Wire.begin();  // Initialize I2C bus
    Wire.setTimeout(200);
    delay(2000);  // Give some time for the serial monitor to open
    scan();
    printConfiguration();
  }

 private:
  static const int MAX_DEVICES_PER_BOARD_TYPE = 8;
  static const int MAX_BOARD_TYPES = 2;
  iBoard* boards[MAX_BOARD_TYPES][MAX_DEVICES_PER_BOARD_TYPE];
  int boardCount[MAX_BOARD_TYPES];
  bool scanned = false;
};
