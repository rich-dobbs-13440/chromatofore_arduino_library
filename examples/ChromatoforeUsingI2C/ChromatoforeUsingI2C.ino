#include <chromatofore.h>
#include <earwig.h>
#include <i2cConfiguration.h>

int i2cActuatorCount = 5; // Update to match your physical configuration.
ChromatoforeFilamentChanger changer(i2cActuatorCount);

// Update based on jumper settings for your specific I2C boards:
int servoBoard0Address = 0x41;
// int servoBoard1Address = ???

int gpioBoard0Address = 0x22;
int gpioBoard1Address = 0x27;
// int gpioBoard2Address = ???


// The pins on the PCA9685 breakout board are numbered from left to right, 
// when the board is positions so the text is upright.

int sb0 = servoBoard0Address;
// int sb1 = servoBoard1Address;
int i2CservoConfiguration[][4] = {
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
  {sb0, 12, 4, PUSHER},
  {sb0, 13, 4, MOVING_CLAMP},
  {sb0, 14, 4, FIXED_CLAMP},   

  // Uncomment to configuration daughter board here for more than 5 actuators 

  // {sb0, 0, 5, PUSHER},
  // {sb0, 1, 5, MOVING_CLAMP},
  // {sb0, 2, 5, FIXED_CLAMP},
  // {sb0, 3, 6, PUSHER},
  // {sb0, 4, 6, MOVING_CLAMP},
  // {sb0, 5, 6, FIXED_CLAMP},  
  // {sb0, 6, 7, PUSHER},
  // {sb0, 7, 7, MOVING_CLAMP},
  // {sb0, 8, 7, FIXED_CLAMP},  
  // {sb0, 9, 8, PUSHER},
  // {sb0, 10, 8, MOVING_CLAMP},
  // {sb0, 11, 8, FIXED_CLAMP}, 
  // {sb0, 12, 9, PUSHER},
  // {sb0, 13, 9, MOVING_CLAMP},
  // {sb0, 14, 9, FIXED_CLAMP},           
}; 

// The pins on the PCF8574 are numbered from right to left 
// when the board is position so the text is upright.  This is 
// in the opposite direction from the servo board:
int gb0 = gpioBoard0Address;
int gb1 = gpioBoard1Address;
//int gb2 = gpioBoard2Address;
int i2cGpioConfiguration[][4] = {
   // I2C, pin, actuator, role
  {gb0, 7, 0, FILAMENT_DETECTOR},
  {gb0, 6, 1, FILAMENT_DETECTOR},
  {gb0, 5, 2, FILAMENT_DETECTOR},
  {gb0, 4, 3, FILAMENT_DETECTOR},
  {gb0, 3, 4, FILAMENT_DETECTOR},
  // {gb0, 2, 5, FILAMENT_DETECTOR},
  // {gb0, 1, 6, FILAMENT_DETECTOR},
  // {gb0, 0, 7, FILAMENT_DETECTOR},


  {gb1, 7, 0, MOVING_CLAMP_LIMIT_SWITCH},
  {gb1, 6, 1, MOVING_CLAMP_LIMIT_SWITCH},
  {gb1, 5, 2, MOVING_CLAMP_LIMIT_SWITCH},
  {gb1, 4, 3, MOVING_CLAMP_LIMIT_SWITCH},  
  {gb1, 3, 4, MOVING_CLAMP_LIMIT_SWITCH}, 
  // {gb0, 2, 5, MOVING_CLAMP_LIMIT_SWITCH},
  // {gb0, 1, 6, MOVING_CLAMP_LIMIT_SWITCH},
  // {gb0, 0, 7, MOVING_CLAMP_LIMIT_SWITCH},  

  // Add configuration of additional daughter GPIO boards here! 
  // One possibility is to add just one more GPIO board and share it for both the filament detector and limit switch: 
  // {gb2, 7, 8, FILAMENT_DETECTOR},
  // {gb2, 6, 9, FILAMENT_DETECTOR},


  // {gb2, 3, 8, MOVING_CLAMP_LIMIT_SWITCH}, 
  // {gb2, 3, 9, MOVING_CLAMP_LIMIT_SWITCH}, 

};

bool setupFailed = false;

void setup() {

  Serial.begin(9600);  // Initialize serial communication
  delay(1000);
  changer.begin();



  int i2cServoCount = sizeof(i2CservoConfiguration) / sizeof(i2CservoConfiguration[0]);
  int i2cGpioCount = sizeof(i2cGpioConfiguration) / sizeof(i2cGpioConfiguration[0]); 
  
  changer.configureForI2C(i2cActuatorCount, i2cServoCount, i2CservoConfiguration, i2cGpioCount, i2cGpioConfiguration);

  // TODO:  Table driven configuration of actuators and other components driven directly f
  //        from the Arduino.
  
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

G1 T E-10 Q1; Extrude 10 mm of filament, feed rate currently ignore.


M1001 T0; Test routine 1

*/

