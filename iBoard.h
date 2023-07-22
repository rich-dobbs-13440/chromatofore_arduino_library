#pragma once

enum BoardType {
  PCA9685_ServoDriver = 0,
  PCF8574_GPIO_Multiplexer = 1,
  // Add more board types here if needed
};

class iBoard {
public:
  virtual void initialize() = 0;
  virtual void printConfig() = 0;
};