#pragma once


enum class SwitchState {
    Triggered,
    Untriggered, 
    Error
};

class ISwitch {
public:
  virtual void begin() = 0;
  virtual SwitchState read() = 0;
};