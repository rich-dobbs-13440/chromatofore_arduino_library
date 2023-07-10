#include "earwig.h"


EarwigFilamentActuator::EarwigFilamentActuator(IServo& pusherServo, IServo& movingClampServo, IServo& fixedClampServo) 
      : pusherServo(pusherServo), movingClampServo(movingClampServo), fixedClampServo(fixedClampServo){

}

void EarwigFilamentActuator::begin() {
    movingClampServo.begin();
    fixedClampServo.begin();
    pusherServo.begin();
    movingClampServo.write(40);
    fixedClampServo.write(40);
    pusherServo.write(40);

    delay(1000);
}