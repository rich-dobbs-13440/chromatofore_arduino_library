#pragma once

#include "iSwitch.h"

typedef unsigned long ExpectedArrivalMillis;

class IServo {
public:
  virtual void begin(int minimumAngle, int maximumAngle, float initialRelativePosition) = 0;
  virtual void loop();
  virtual ExpectedArrivalMillis write(int angle) = 0;
  virtual ExpectedArrivalMillis position(float relativePosition) = 0;
  virtual void atEase() = 0;
  virtual void detach() = 0;
  virtual void dump() = 0;
  virtual void setMinimumAngle(int minimumAngle)  = 0;
  virtual void setMaximumAngle(int maximumAngle)  = 0;
  virtual int getMinimumAngle() = 0;
  virtual int getMaximumAngle() = 0;
  virtual int getCurrentAngle() = 0;
  virtual void home(ISwitch* limitSwitch)  = 0;
  virtual bool isIdle() = 0; 
  virtual bool isFailed() = 0;
  virtual void reset() = 0;
};

