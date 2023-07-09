#ifndef ISERVO_H
#define ISERVO_H

class IServo {
public:
  virtual void write(int angle) = 0;
  virtual void detach() = 0;
};


#endif