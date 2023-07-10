#include <chromatofore.h>
#include <earwig.h>
#include <standardServo.h>
/*
On the Arduino Mega 2560, the pins that support PWM (Pulse Width Modulation) are as follows:

Digital Pins:

Pin 2 to Pin 13
Pin 44 to Pin 46
Pin 48 to Pin 49
Pin 51 to Pin 53

*/

// Servo pins

const int EXTRUDER_ENGAGE_PIN = 11;

const int FILAMENT_MOVE_PIN_1 = 8;
const int FILAMENT_CLAMP_PIN_1 = 9;
const int FILAMENT_ROTATE_PIN_1 = 10;
const int FILAMENT_FIXED_CLAMP_PIN_1 = 7;


ChromatoforeFilamentChanger changer(20);

StandardServo pusherServo1(FILAMENT_MOVE_PIN_1);
StandardServo movingClampServo1(FILAMENT_CLAMP_PIN_1);
StandardServo fixedClampServo1(FILAMENT_FIXED_CLAMP_PIN_1);

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
