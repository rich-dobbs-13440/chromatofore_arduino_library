#ifndef ISERVO_H
#define ISERVO_H

class IServo {
public:
  virtual void begin(int minimumAngle, int maximumAngle, float initialRelativePosition) = 0;
  virtual void write(int angle) = 0;
  virtual void position(float relativePosition) = 0;
  virtual void detach() = 0;
  virtual void dump() = 0;
};


#endif