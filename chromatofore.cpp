#include "chromatofore.h"

#include <EEPROM.h>

#include "debugLog.h"
#include "pca9685Servo.h"
#include "pcf8574FilamentDetector.h"
#include "gcodeSerialHandler.h"



ChromatoforeFilamentChanger::ChromatoforeFilamentChanger(int size)
    : actuatorArraySize(size) {
  actuatorArray = new EarwigFilamentActuator *[actuatorArraySize]();
}

ChromatoforeFilamentChanger::~ChromatoforeFilamentChanger() {
  delete[] actuatorArray;
  if (i2cActuators != nullptr) {
    delete[] i2cActuators;
    i2cActuators = nullptr;
  }
  if (i2cServos != nullptr) {
    delete[] i2cServos;
    i2cServos = nullptr;
  }
  if (i2cFilamentDetectors != nullptr) {
    delete[] i2cFilamentDetectors;
    i2cFilamentDetectors = nullptr;
  }
  if (i2cConfiguration != nullptr) {
    delete i2cConfiguration;
    i2cConfiguration = nullptr;
  }
  if (serialHandler != nullptr) {
    delete serialHandler;
    serialHandler = nullptr;
  }
}

void ChromatoforeFilamentChanger::addActuator(
    int index, EarwigFilamentActuator *actuator) {
  if (index >= 0 && index < actuatorArraySize) {
    actuatorArray[index] = actuator;
  }
}

void ChromatoforeFilamentChanger::removeActuator(int index) {
  if (index >= 0 && index < actuatorArraySize) {
    actuatorArray[index] = nullptr;
  }
}

EarwigFilamentActuator *ChromatoforeFilamentChanger::getActuator(int index) {
  if (index >= 0 && index < actuatorArraySize) {
    return actuatorArray[index];
  }
  return nullptr;
}

void ChromatoforeFilamentChanger::begin() {


  debugLog("--------------");
  debugLog("Chromatofore Version:", version);
  debugLog("Upload Date:", __DATE__);
  debugLog("Upload Time:", __TIME__);
  debugLog("Baud Rate:", baudRate);

  int restartCount = EEPROM.read(0);
  debugLog("Restart Count:", restartCount);
  restartCount++;
  EEPROM.write(0, restartCount);

  debugLog("Millis:", millis());

  for (int tool = 0; tool < actuatorArraySize; ++tool) {
    if (actuatorArray[tool] != nullptr) {
      uint8_t minB = getMinimumAngleB(tool);
      uint8_t maxB = getMaximumAngleB(tool);
      uint8_t minC = getMinimumAngleC(tool);
      uint8_t maxC = getMaximumAngleC(tool);
      uint8_t minX = getMinimumAngleX(tool);
      uint8_t maxX = getMaximumAngleX(tool);
      actuatorArray[tool]->begin(minB, maxB, minC, maxC, minX, maxX);
    }
  }

  auto gcodeSerialHandler = new GcodeSerialHandler();
  gcodeSerialHandler->initialize(*this);
  serialHandler = gcodeSerialHandler;
}

void ChromatoforeFilamentChanger::loop() {
  EarwigFilamentActuator *pActuator = getActuator(currentFilament);
  if (pActuator) {
    pActuator->loop();
  }
  serialHandler->handleSerial();
}



void ChromatoforeFilamentChanger::rememberMinimumAngleForTool(int tool, float b,
                                                              float c,
                                                              float x) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In rememberMaximumAngleForTool, invalid tool index", tool,
             "No values remembered");
    return;
  }

  // Calculate the EEPROM offset based on the tool index
  int offset = TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6);

  // Check if b value is specified and store it
  if (!isnan(b)) {
    if (b >= 0 && b <= 180) {
      uint8_t minB = static_cast<uint8_t>(b);
      EEPROM.put(offset, minB);
      debugLog("Current b mininum", getMinimumAngleB(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, b is out of range 0-180", b,
               "No value remembered");
    }
  }

  // Check if c value is specified and store it
  if (!isnan(c)) {
    if (c >= 0 && c <= 180) {
      uint8_t minC = static_cast<uint8_t>(c);
      EEPROM.put(offset + sizeof(uint8_t), minC);
      debugLog("Current c mininum", getMinimumAngleC(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, c is out of range 0-180", c,
               "No value remembered");
    }
  }

  // Check if x value is specified and store it
  if (!isnan(x)) {
    if (x >= 0 && x <= 180) {
      uint8_t minX = static_cast<uint8_t>(x);
      EEPROM.put(offset + sizeof(uint8_t) * 2, minX);
      debugLog("Current x mininum", getMinimumAngleX(tool));
    } else {
      debugLog("In rememberMinimumAngleForTool, x is out of range 0-180", x,
               "No value remembered");
    }
  }
}

void ChromatoforeFilamentChanger::rememberMaximumAngleForTool(int tool, float b,
                                                              float c,
                                                              float x) {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In rememberMaximumAngleForTool, invalid tool index", tool,
             "No values remembered");
    return;
  }
  // Calculate the EEPROM offset based on the tool index
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 3;

  // Check if b value is specified and store it
  if (!isnan(b)) {
    if (b >= 0 && b <= 180) {
      uint8_t maxB = static_cast<uint8_t>(b);
      EEPROM.put(offset, maxB);
      debugLog("Current b maximum", getMaximumAngleB(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, b is out of range 0-180", b,
               "No value remembered");
    }
  }

  // Check if c value is specified and store it
  if (!isnan(c)) {
    if (c >= 0 && c <= 180) {
      uint8_t maxC = static_cast<uint8_t>(c);
      EEPROM.put(offset + sizeof(uint8_t), maxC);
      debugLog("Current c maximum", getMaximumAngleC(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, c is out of range 0-180", c,
               "No value remembered");
    }
  }

  // Check if x value is specified and store it
  if (!isnan(x)) {
    if (x >= 0 && x <= 180) {
      uint8_t maxX = static_cast<uint8_t>(x);
      EEPROM.put(offset + sizeof(uint8_t) * 2, maxX);
      debugLog("Current x maximum", getMaximumAngleX(tool));
    } else {
      debugLog("In rememberMaximumAngleForTool, x is out of range 0-180", x,
               "No value remembered");
    }
  }
}

int ChromatoforeFilamentChanger::getMinimumAngleB(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleB, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset = TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6);

  // Retrieve and return the stored minimum angle for axis B
  uint8_t b;
  EEPROM.get(offset, b);
  return b;
}

int ChromatoforeFilamentChanger::getMinimumAngleC(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleC, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t);

  // Retrieve and return the stored minimum angle for axis C
  uint8_t c;
  EEPROM.get(offset, c);
  return c;
}

int ChromatoforeFilamentChanger::getMinimumAngleX(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMinimumAngleX, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index and named constant
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 2;

  // Retrieve and return the stored minimum angle for axis X
  uint8_t x;
  EEPROM.get(offset, x);
  return x;
}

int ChromatoforeFilamentChanger::getMaximumAngleB(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleB, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 3;

  // Retrieve and return the stored maximum angle for axis B
  uint8_t b;
  EEPROM.get(offset, b);
  return b;
}

int ChromatoforeFilamentChanger::getMaximumAngleC(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleC, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 4;

  // Retrieve and return the stored maximum angle for axis C
  uint8_t c;
  EEPROM.get(offset, c);
  return c;
}

int ChromatoforeFilamentChanger::getMaximumAngleX(int tool) const {
  if (tool < 0 || tool >= actuatorArraySize) {
    debugLog("In getMaximumAngleX, invalid tool index", tool);
    return -1;
  }

  // Calculate the EEPROM offset based on the tool index, named constant, and
  // minimum angle offset
  int offset =
      TOOL_EEPROM_OFFSET + (tool * sizeof(uint8_t) * 6) + sizeof(uint8_t) * 5;

  // Retrieve and return the stored maximum angle for axis X
  uint8_t x;
  EEPROM.get(offset, x);
  return x;
}

void ChromatoforeFilamentChanger::initializeEEPROM() {
  // Initialize the initial block to zero
  for (int i = 0; i < TOOL_EEPROM_OFFSET; i++) {
    EEPROM.write(i, 0);
  }

  // Initialize each tool with named constants for each axis
  for (int tool = 0; tool < actuatorArraySize; tool++) {
    rememberMinimumAngleForTool(tool, DEFAULT_MINIMUM_ANGLE_B,
                                DEFAULT_MINIMUM_ANGLE_C,
                                DEFAULT_MINIMUM_ANGLE_X);
    rememberMaximumAngleForTool(tool, DEFAULT_MAXIMUM_ANGLE_B,
                                DEFAULT_MAXIMUM_ANGLE_C,
                                DEFAULT_MAXIMUM_ANGLE_X);
  }
}

bool ChromatoforeFilamentChanger::configureForI2C(int i2cActuatorCount,
                                                  int servoConfiguration[][4],
                                                  int gpioConfiguration[][4]) {
  i2cConfiguration = new I2CConfiguration;
  i2cConfiguration->begin();

  i2cActuators = new EarwigFilamentActuator[i2cActuatorCount];
  this->i2cActuatorCount = i2cActuatorCount;
  i2cServoCount = i2cActuatorCount * SERVOS_PER_EARWIG_ACTUATOR;
  i2cServos = new Pca9685PinServo[i2cServoCount];
  i2cFilamentDetectors = new Pcf8574FilamentDetector[i2cActuatorCount];

  for (int actuatorIndex = 0; actuatorIndex < i2cActuatorCount;
       actuatorIndex++) {
    Pca9685ServoInfo pusher = getPca9685ServoInfo(
        servoConfiguration, i2cServoCount, actuatorIndex, PUSHER);
    debugLog("pusher servoIndex:", pusher.servoIndex,
             " ic2 address:", pusher.i2cAddress, "pin:", pusher.pin);
    if (pusher.servoIndex < 0) {
      debugLog("Missing configuration for actuator: ", actuatorIndex,
               " Role = PUSHER");
      return false;
    }
    Pca9685PinServo *pusherServo = &i2cServos[pusher.servoIndex];
    pusherServo->initialize(
        "pusherServo",
        i2cConfiguration->getPca9685ServoDriverFromAddress(pusher.i2cAddress),
        pusher.pin);

    Pca9685ServoInfo movingClamp = getPca9685ServoInfo(
        servoConfiguration, i2cServoCount, actuatorIndex, MOVING_CLAMP);
    debugLog("movingClamp servoIndex:", movingClamp.servoIndex,
             " ic2 address:", movingClamp.i2cAddress, "pin:", movingClamp.pin);
    if (movingClamp.servoIndex < 0) {
      debugLog("Missing configuration for actuator: ", actuatorIndex,
               " Role = MOVING_CLAMP");
      return false;
    }
    Pca9685PinServo *movingClampServo = &i2cServos[movingClamp.servoIndex];
    movingClampServo->initialize(
        "movingClampServo",
        i2cConfiguration->getPca9685ServoDriverFromAddress(
            movingClamp.i2cAddress),
        movingClamp.pin);

    Pca9685ServoInfo fixedClamp = getPca9685ServoInfo(
        servoConfiguration, i2cServoCount, actuatorIndex, FIXED_CLAMP);
    debugLog("fixedClamp servoIndex:", fixedClamp.servoIndex,
             " ic2 address:", fixedClamp.i2cAddress, "pin:", fixedClamp.pin);
    if (fixedClamp.servoIndex < 0) {
      debugLog("Missing configuration for actuator: ", actuatorIndex,
               " Role = FIXED_CLAMP");
      return false;
    }
    Pca9685PinServo *fixedClampServo = &i2cServos[fixedClamp.servoIndex];
    fixedClampServo->initialize(
        "fixedClampServo",
        i2cConfiguration->getPca9685ServoDriverFromAddress(
            fixedClamp.i2cAddress),
        fixedClamp.pin);

    Pcf8574SwitchInfo filamentDetectorInfo = getPcf8574SwitchInfo(
        gpioConfiguration, i2cActuatorCount, actuatorIndex, FILAMENT_DETECTOR);
    if (filamentDetectorInfo.actuator < 0) {
      debugLog("Missing configuration for actuator: ", actuatorIndex,
               "Role = FILAMENT_DETECTOR");
      return false;
    }

    PCF8574GPIOMultiplexer *filamentDetectorMultiplexer =
        i2cConfiguration->getGpioMultiplexerFromAddress(
            filamentDetectorInfo.i2cAddress);
    if (filamentDetectorMultiplexer == nullptr) {
      debugLog("Can't find a PCF8574 GPIO Multiplexer with addrees of ",
               filamentDetectorInfo.i2cAddress);
      return false;
    }
    Pcf8574FilamentDetector *i2cFilamentDetector =
        &i2cFilamentDetectors[filamentDetectorInfo.actuator];
    i2cFilamentDetector->initialize(filamentDetectorInfo,
                                    *filamentDetectorMultiplexer);

    i2cActuators[actuatorIndex].initialize(*pusherServo, *movingClampServo,
                                           *fixedClampServo,
                                           *i2cFilamentDetector);
    addActuator(actuatorIndex, &i2cActuators[actuatorIndex]);
  }
  return true;
}
