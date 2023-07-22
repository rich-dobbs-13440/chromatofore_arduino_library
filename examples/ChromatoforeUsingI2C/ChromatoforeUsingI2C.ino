#include <chromatofore.h>
#include <earwig.h>
#include <i2cConfiguration.h>
#include <pca9685Servo.h>


I2CConfiguration i2cConfiguration;
ChromatoforeFilamentChanger changer(20);

const int I2C_ACTUATOR_COUNT = 4;

const int SERVOS_PER_ACTUATOR = 3;
Pca9685PinServo i2cServos[I2C_ACTUATOR_COUNT*SERVOS_PER_ACTUATOR];


const int PUSHER = 0;
const int MOVING_CLAMP = 1;
const int FIXED_CLAMP = 2;

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


EarwigFilamentActuator iC2Actuators[I2C_ACTUATOR_COUNT];

Pca9685PinServo pusherServo0;
Pca9685PinServo movingClampServo0;
Pca9685PinServo fixedClampServo0;


EarwigFilamentActuator actuator0;

bool setupFailed = false;

void setup() {
  int i2cServoCount = sizeof(servoConfiguration) / sizeof(servoConfiguration[0]);
  assert(i2cServoCount == sizeof(i2cServos)); // Bad servoConfiguration - size mismatch
  Serial.begin(9600);  // Initialize serial communication
  delay(2000);
  Serial.println("Start i2cConfiguration.begin()");
  i2cConfiguration.begin();
  Serial.println("Done with i2cConfiguration.begin()");

  for (int actuatorIndex = 0; actuatorIndex < i2cServoCount; actuatorIndex++) {
    Pca9685ServoInfo pusher =  getPca9685ServoInfo(
      servoConfiguration, I2C_ACTUATOR_COUNT, actuatorIndex, PUSHER  
    );
    if (pusher.servoIndex < 0) {
      setupFailed = true;
      debugLog("Missing configuration for actuator: ", actuatorIndex, " Role = PUSHER");
      return;
    }
    Pca9685PinServo* pusherServo = &i2cServos[pusher.servoIndex];
    pusherServo->initialize("pusherServo", i2cConfiguration.getPca9685ServoDriverFromAddress(pusher.i2cAddress), pusher.pin);

    Pca9685ServoInfo movingClamp = getPca9685ServoInfo(
      servoConfiguration, I2C_ACTUATOR_COUNT, actuatorIndex, MOVING_CLAMP  
    );
    if (movingClamp.servoIndex < 0) {
      setupFailed = true;
      debugLog("Missing configuration for actuator: ", actuatorIndex, " Role = MOVING_CLAMP");
      return;
    } 
    Pca9685PinServo* movingClampServo = &i2cServos[movingClamp.servoIndex];
    movingClampServo->initialize("movingClampServo", i2cConfiguration.getPca9685ServoDriverFromAddress(movingClamp.i2cAddress), movingClamp.pin);


    Pca9685ServoInfo fixedClamp = getPca9685ServoInfo(
      servoConfiguration, I2C_ACTUATOR_COUNT, actuatorIndex, FIXED_CLAMP  
    ); 
    if (fixedClamp.servoIndex < 0) {
      setupFailed = true;
      debugLog("Missing configuration for actuator: ", actuatorIndex, " Role = FIXED_CLAMP");
      return;
    } 
    Pca9685PinServo* fixedClampServo = &i2cServos[movingClamp.servoIndex];
    fixedClampServo->initialize("fixedClampServo", i2cConfiguration.getPca9685ServoDriverFromAddress(fixedClamp.i2cAddress), fixedClamp.pin);

    iC2Actuators[actuatorIndex].initialize(*pusherServo, *movingClampServo, *fixedClampServo);
    changer.addActuator(actuatorIndex, &iC2Actuators[actuatorIndex]);
  }  
  changer.setCurrentFilament(1);  // Todo:  Add filament detector to show what filament is loaded.
  changer.begin();
}

void loop() {
  changer.loop();
}
