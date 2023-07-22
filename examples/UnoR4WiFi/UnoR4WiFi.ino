/* This configuration will use servos connected to a PCA9685 servo controller board. */


#include <chromatofore.h>
#include <earwig.h>
#include <arduinoPinServo.h>

#include <Wire.h>
#include <i2cConfiguration.h>

I2CConfiguration i2cConfiguration;

// Board: [0][0]: PCA9685ServoDriver, I2C address: 0x41
// Board: [1][0]: PCF8574GPIOMultiplexer, I2C address: 0x22
// Board: [1][1]: PCF8574GPIOMultiplexer, I2C address: 0x24


ChromatoforeFilamentChanger changer(20);

// Just keep the actuator_1 for compile checks: 

const int FILAMENT_MOVE_PIN_1 = 8;
const int FILAMENT_CLAMP_PIN_1 = 9;
const int FILAMENT_ROTATE_PIN_1 = 10;
const int FILAMENT_FIXED_CLAMP_PIN_1 = 7;

ArduinoPinServo pusherServo1("pusherServo1", FILAMENT_MOVE_PIN_1);
ArduinoPinServo movingClampServo1("movingClampServo1", FILAMENT_CLAMP_PIN_1);
ArduinoPinServo fixedClampServo1("fixedClampServo1", FILAMENT_FIXED_CLAMP_PIN_1);

EarwigFilamentActuator actuator_1(pusherServo1, movingClampServo1, fixedClampServo1);



void setup() {

  changer.addActuator(1, &actuator_1);
  changer.setCurrentFilament(1);  // Todo:  Add filament detector to show what filament is loaded.
  changer.begin();
  debugLog("sizeof(changer)", sizeof(changer));
}

void loop() {
  changer.loop();
  //delay(1);
}
