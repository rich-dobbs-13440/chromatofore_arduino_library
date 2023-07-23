#include "pcf8574Switch.h"


// gpioConfiguration  is I2C address, pin, actuator, role

const int ADDRESS = 0;
const int PIN = 1;
const int ACTUATOR = 2;
const int ROLE = 3;


Pcf8574SwitchInfo getPcf8574SwitchInfo(int gpioConfiguration[][4], int numRows, int actuator, int role) {
    
    for (int row = 0; row < numRows; row++) {
        if (gpioConfiguration[row][ACTUATOR] == actuator && gpioConfiguration[row][ROLE] == role) {
            Pcf8574SwitchInfo result;
            
            result.i2cAddress = gpioConfiguration[row][ADDRESS];
            result.pin = gpioConfiguration[row][PIN];
            result.actuator = gpioConfiguration[row][ACTUATOR];
            result.role = gpioConfiguration[row][ROLE];
            return result;
        }
    }
    return {-1, -1, -1, -1};
}