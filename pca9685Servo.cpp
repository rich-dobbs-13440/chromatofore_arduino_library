#include "pca9685Servo.h"


Pca9685ServoInfo getPca9685ServoInfo(int servoConfiguration[][4], int numRows, int actuator, int role) {
    for (int servoIndex = 0; servoIndex < numRows; servoIndex++) {
        if (servoConfiguration[servoIndex][2] == actuator && servoConfiguration[servoIndex][3] == role) {
            return {servoIndex, servoConfiguration[servoIndex][0], servoConfiguration[servoIndex][1]};
        }
    }

    // No match found
    return {-1, -1, -1}; 
}