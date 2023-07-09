#include "earwig.h"


EarwigFilamentActuator::EarwigFilamentActuator(IServo& pusherServo, IServo& movingClampServo, IServo& fixedClampServo) 
      : pusherServo(pusherServo), movingClampServo(movingClampServo), fixedClampServo(fixedClampServo){

}

void EarwigFilamentActuator::begin() {
    movingClampServo.write(10);
    fixedClampServo.write(10);
    pusherServo.write(80);
}