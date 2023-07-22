#include <chromatofore.h>
#include <earwig.h>
#include <i2cConfiguration.h>

const int I2C_ACTUATOR_COUNT = 4;
ChromatoforeFilamentChanger changer(I2C_ACTUATOR_COUNT);

EarwigFilamentActuator iC2Actuators[I2C_ACTUATOR_COUNT];
Pca9685PinServo i2cServos[I2C_ACTUATOR_COUNT*SERVOS_PER_ACTUATOR];

I2CConfiguration i2cConfiguration;


int servoConfiguration[][4] = {
  // I2C, pin, actuator, role
  {0x41, 0, 0, PUSHER},
  {0x41, 1, 0, MOVING_CLAMP},
  {0x41, 2, 0, FIXED_CLAMP},
  {0x41, 3, 1, PUSHER},
  {0x41, 4, 1, MOVING_CLAMP},
  {0x41, 5, 1, FIXED_CLAMP},  
  {0x41, 6, 2, PUSHER},
  {0x41, 7, 2, MOVING_CLAMP},
  {0x41, 8, 2, FIXED_CLAMP},  
  {0x41, 9, 3, PUSHER},
  {0x41, 10, 3, MOVING_CLAMP},
  {0x41, 11, 3, FIXED_CLAMP},        
}; 

bool setupFailed = false;

void setup() {

  Serial.begin(9600);  // Initialize serial communication
  delay(2000);

  int i2cServoCount = sizeof(servoConfiguration) / sizeof(servoConfiguration[0]);
  if (i2cServoCount != I2C_ACTUATOR_COUNT*SERVOS_PER_ACTUATOR) {;
    debugLog("Bad servoConfiguration - size mismatch: ", i2cServoCount, I2C_ACTUATOR_COUNT*SERVOS_PER_ACTUATOR);
  };//  
  Serial.println("Start i2cConfiguration.begin()");
  Serial.flush();
  i2cConfiguration.begin();
  Serial.println("Done with i2cConfiguration.begin()");
  changer.configureForI2C(I2C_ACTUATOR_COUNT, i2cServoCount, servoConfiguration, i2cConfiguration, iC2Actuators, i2cServos);
  
  changer.setCurrentFilament(1);  // Todo:  Add filament detector to show what filament is loaded.
  changer.begin();
}
/*
The following GCODE parsing is currently implemented:


G1 T0 B10 ; For actuator 0, move the fixed clamp servo to an angle of 10 degrees

G1 T1 C10 ; For actuator 1, Move the clamp servo to an angle of 10 degrees

G1 T2 X10 ; For actuator 2, move the pusher servo to an angle of 10 degrees

G10 T3 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles
for tool 3

G10 T0 L1 B101 C101 X145 ; Remember maximum values for the B, C, and X axis
angles for tool 0

G28 T1 B C X ; Home all axes, say for loading or unloading filament.

The following commands are parsed, but the functionality is not yet implemented:


G1 T1 E10 F10 : Extrude 10 mm of filament, feed rate currently ignore.  (Parsed by
functionality is not yet implemented in the actuator!)

*/

void loop() {
  changer.loop();
}
