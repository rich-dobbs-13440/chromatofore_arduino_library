#include <chromatofore.h>
#include <earwig.h>
#include <i2cConfiguration.h>

const int I2C_ACTUATOR_COUNT = 4; // Update to match your physical configuration.
ChromatoforeFilamentChanger changer(I2C_ACTUATOR_COUNT);

// Update based on jumper settings for your specific I2C boards:
int servoBoard0Address = 0x41;
int gpioBoard0Address = 0x22;


// The pins on the PCA9685 breakout board are number from left to right, 
// when the board is positions so the text is upright.

int sb0 = servoBoard0Address;
int servoConfiguration[][4] = {
  // I2C, pin, actuator, role
  {sb0, 0, 0, PUSHER},
  {sb0, 1, 0, MOVING_CLAMP},
  {sb0, 2, 0, FIXED_CLAMP},
  {sb0, 3, 1, PUSHER},
  {sb0, 4, 1, MOVING_CLAMP},
  {sb0, 5, 1, FIXED_CLAMP},  
  {sb0, 6, 2, PUSHER},
  {sb0, 7, 2, MOVING_CLAMP},
  {sb0, 8, 2, FIXED_CLAMP},  
  {sb0, 9, 3, PUSHER},
  {sb0, 10, 3, MOVING_CLAMP},
  {sb0, 11, 3, FIXED_CLAMP}, 
  // Add configuration of daughter board here!       
}; 

// The pins on the PCF8574 are numbered in the opposite direction from the servo board:
int gb0 = gpioBoard0Address;
int gpioConfiguration[][4] = {
   // I2C, pin, actuator, role
  {gb0, 7, 0, FILAMENT_DETECTOR},
  {gb0, 6, 1, FILAMENT_DETECTOR},
  {gb0, 5, 2, FILAMENT_DETECTOR},
  {gb0, 4, 3, FILAMENT_DETECTOR},
  // Add configuration of second daughter GPIO boards here!  
};

bool setupFailed = false;

void setup() {

  Serial.begin(9600);  // Initialize serial communication
  delay(2000);

  int i2cServoCount = sizeof(servoConfiguration) / sizeof(servoConfiguration[0]);
  if (i2cServoCount != I2C_ACTUATOR_COUNT*SERVOS_PER_EARWIG_ACTUATOR) {;
    debugLog("Bad servoConfiguration - size mismatch: ", i2cServoCount, I2C_ACTUATOR_COUNT*SERVOS_PER_EARWIG_ACTUATOR);
  };//  
  
  changer.configureForI2C(I2C_ACTUATOR_COUNT, servoConfiguration, gpioConfiguration);
  changer.begin();
}

void loop() {
  changer.loop();
}

/*

Configuration debugging commands:

 - Use one at a time in the serial monitor.

; Actuator 0

  ; Test pusher servo
    G1 T0 X50; Toward fixed clamp
    G1 T0 X80; Mid
    G1 T0 X100; Toward outlet clamp

  ; Test fixed clamp
    G1 T0 B10; Open
    G1 T0 B50; Mid
    G1 T0 B100; Locked

  ; Test Moving clamp
    G1 T0 C10; Open
    G1 T0 B50; Mid  
    G1 T0 C100; Locked

  ; Retract filament 
    G1 T0 E-100 
  ; Advance filament
    G1 T0 E100 

; Actuator 1

  ; Test pusher servo
    G1 T1 X50; Toward fixed clamp
    G1 T1 X80; Mid
    G1 T1 X100; Toward outlet clamp

  ; Test fixed clamp
    G1 T1 B10; Open
    G1 T1 B50; Mid
    G1 T1 B80; Locked

  ; Test Moving clamp
    G1 T1 C10; Open
    G1 T1 B50; Mid  
    G1 T1 C100; Locked


; Actuator 2

  ; Test pusher servo
    G1 T2 X50; Toward fixed clamp
    G1 T2 X80; Mid
    G1 T2 X100; Toward outlet clamp

  ; Test fixed clamp
    G1 T2 B10; Open
    G1 T2 B50; Mid
    G1 T2 B80; Locked

  ; Test Moving clamp
    G1 T2 C10; Open
    G1 T2 B50; Mid  
    G1 T2 C100; Locked

; Actuator 3

  ; Test pusher servo
    G1 T3 X50; Toward fixed clamp
    G1 T3 X80; Mid
    G1 T3 X100; Toward outlet clamp

  ; Test fixed clamp
    G1 T3 B10; Open
    G1 T3 B50; Mid
    G1 T3 B80; Locked

  ; Test Moving clamp
    G1 T3 C10; Open
    G1 T3 B50; Mid  
    G1 T3 C100; Locked    



G10 T3 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles
for tool 3

G10 T0 L1 B101 C101 X145 ; Remember maximum values for the B, C, and X axis
angles for tool 0

G28 T1 B C X ; Home all axes, say for loading or unloading filament.


G1 T1 E10 F10 : Extrude 10 mm of filament, feed rate currently ignore. 



*/

