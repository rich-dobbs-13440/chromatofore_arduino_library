#pragma once



enum class FilamentDetectorState {
    Detected,
    Undetected, 
    Error
};

class IFilamentDetector {
public:
  virtual void begin() = 0;
  virtual FilamentDetectorState read() = 0;
};