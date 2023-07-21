#include <Wire.h>
#include <PCF8574.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>

const int MAX_DEVICES_PER_BOARD_TYPE = 8;  // Maximum number of devices per board
const int MAX_BOARD_TYPES = 2;             // Maximum number of boards



enum BoardType {
  PCA9685_ServoDriver = 0,
  PCF8574_GPIO_Multiplexer = 1,
  // Add more board types here if needed
};

class Board {
public:
  virtual void initialize() = 0;
  virtual void printConfig() = 0;
};

class PCA9685ServoDriver : public Board {
public:
  PCA9685ServoDriver(byte i2cAddress)
    : i2cAddress(i2cAddress) {}
  void initialize() override {
    // Implementation specific to PCA9685 Servo Driver board initialization
    // Initialize servo driver board here
    pwmServoDriver = Adafruit_PWMServoDriver(i2cAddress);
    pwmServoDriver.begin();
    pwmServoDriver.setOscillatorFrequency(27000000);
    pwmServoDriver.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  }
  void printConfig() override {
    Serial.print("PCA9685ServoDriver, I2C address: 0x");
    Serial.println(i2cAddress, HEX);
  }
  // Add other PCA9685 specific methods or properties here
private:
  byte i2cAddress;
  Adafruit_PWMServoDriver pwmServoDriver;

  // Depending on your servo make, the pulse width min and max may vary, you
  // want these to be as small/large as possible without hitting the hard stop
  // for max range. You'll have to tweak them as necessary to match the servos you
  // have!
  static const int SERVOMIN = 150;   // This is the 'minimum' pulse length count (out of 4096)
  static const int SERVOMAX = 600;   // This is the 'maximum' pulse length count (out of 4096)
  static const int USMIN = 600;      // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
  static const int USMAX = 2400;     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
  static const int SERVO_FREQ = 50;  // Analog servos run at ~50 Hz updates
};

class PCF8574GPIOMultiplexer : public Board {
public:
  PCF8574GPIOMultiplexer(byte i2cAddress)
    : i2cAddress(i2cAddress), pcf8574(i2cAddress) {}
  void initialize() override {
    // // Initialize Wire library and set the pin configuration of the PCF8574
    // Wire.begin();
    // Wire.beginTransmission(i2cAddress);
    // Wire.write(0xFF);  // Set all pins as inputs (high impedance)
    // Wire.endTransmission();


    pcf8574.pinMode(P0, INPUT);
    pcf8574.pinMode(P1, INPUT);
    pcf8574.pinMode(P2, INPUT);
    pcf8574.pinMode(P3, INPUT);
    pcf8574.pinMode(P4, INPUT);
    pcf8574.pinMode(P5, INPUT);
    pcf8574.pinMode(P6, INPUT);
    pcf8574.pinMode(P7, INPUT);

    Serial.print("Init pcf8574...");
    if (pcf8574.begin()) {
      Serial.println("OK");
    } else {
      Serial.println("KO");
    }
  }
  void printConfig() override {
    Serial.print("PCF8574GPIOMultiplexer, I2C address: 0x");
    Serial.println(i2cAddress, HEX);
  }
  bool readPin(byte pinNumber) {
    if (pinNumber >= 0 && pinNumber < 8) {
      // Read the state of all pins from the PCF8574
      Wire.requestFrom(i2cAddress, static_cast<int>(1));
      if (Wire.available()) {
        byte pinStates = Wire.read();

        // Extract the state of the specified pin (0 = LOW, 1 = HIGH)
        return bitRead(pinStates, pinNumber);
      } else {
        Serial.println("Wire.available() is false!");
      }
    }

    // If the pin number is invalid, return false
    return false;
  }

  void readAndPrintPins() {
    Serial.print("I2C: 0x");
    Serial.print(i2cAddress, HEX);
    Serial.print("  ");
    pcf8574.readBuffer();
    // read value on buffer than reset value for that pin
    uint8_t val = pcf8574.digitalRead(P0);
    if (val == HIGH) {
      Serial.print("P0 HIGH  ");
    } else {
      Serial.print("P0 LOW   ");
    }
    val = pcf8574.digitalRead(P1);
    if (val == HIGH) {
      Serial.print("P1 HIGH  ");
    } else {
      Serial.print("P1 LOW   ");
    }
    val = pcf8574.digitalRead(P2);
    if (val == HIGH) {
      Serial.print("P2 HIGH  ");
    } else {
      Serial.print("P2 LOW   ");
    }
    val = pcf8574.digitalRead(P3);
    if (val == HIGH) {
      Serial.print("P3 HIGH  ");
    } else {
      Serial.print("P3 LOW   ");
    }
    val = pcf8574.digitalRead(P4);
    if (val == HIGH) {
      Serial.print("P4 HIGH  ");
    } else {
      Serial.print("P4 LOW   ");
    }
    val = pcf8574.digitalRead(P5);
    if (val == HIGH) {
      Serial.print("P5 HIGH  ");
    } else {
      Serial.print("P5 LOW   ");
    }
    val = pcf8574.digitalRead(P6);
    if (val == HIGH) {
      Serial.print("P6 HIGH  ");
    } else {
      Serial.print("P6 LOW   ");
    }
    val = pcf8574.digitalRead(P7);
    if (val == HIGH) {
      Serial.print("P7 HIGH  ");
    } else {
      Serial.print("P7 LOW   ");
    }
    Serial.println();
  }

  void pcf8574Loop(int seconds) {
    int intervalSeconds = 2;
    int loopCount = seconds / intervalSeconds;
    for (int loop = 0; loop < loopCount; loop++) {
      readAndPrintPins();
      delay(1000 * intervalSeconds);
    }
  }
private:
  byte i2cAddress;
  PCF8574 pcf8574;
};

class I2CConfig {
public:
  I2CConfig() {
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < MAX_DEVICES_PER_BOARD_TYPE; j++) {
        boards[i][j] = nullptr;
      }
      boardCount[i] = 0;
    }
  }

  bool addBoard(BoardType type, byte i2cAddress) {
    if (type == BoardType::PCA9685_ServoDriver) {
      if (boardCount[BoardType::PCA9685_ServoDriver] < MAX_DEVICES_PER_BOARD_TYPE) {
        PCA9685ServoDriver* pBoard = new PCA9685ServoDriver(i2cAddress);
        pBoard->initialize();
        boards[BoardType::PCA9685_ServoDriver][boardCount[BoardType::PCA9685_ServoDriver]] = pBoard;
        boardCount[BoardType::PCA9685_ServoDriver]++;
        return true;
      } else {
        Serial.println("Too many PCA9685 Servo Drivers, can't add another one.");
        return false;
      }
    } else if (type == BoardType::PCF8574_GPIO_Multiplexer) {
      if (boardCount[BoardType::PCA9685_ServoDriver] < MAX_DEVICES_PER_BOARD_TYPE) {
        PCF8574GPIOMultiplexer* pBoard = new PCF8574GPIOMultiplexer(i2cAddress);
        pBoard->initialize();
        boards[BoardType::PCF8574_GPIO_Multiplexer][boardCount[BoardType::PCF8574_GPIO_Multiplexer]] = pBoard;
        boardCount[BoardType::PCF8574_GPIO_Multiplexer]++;
        return true;
      } else {
        Serial.println("Too many PCF8574 GPIO_Multiplexers, can't add another one.");
        return false;
      }
    }
    Serial.print("Unknown board type: ");
    Serial.println(type);
  }

  void printConfig() {
    Serial.println("I2C bus configuration:");
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < boardCount[i]; j++) {
        Serial.print("Board: [");
        Serial.print(i);
        Serial.print("][");
        Serial.print(j);
        Serial.print("]: pointer: ");

        Serial.print(reinterpret_cast<int>(boards[i][j]), HEX);
        Serial.print(" ");
        boards[i][j]->printConfig();
      }
    }
  }

  void reset() {
    for (int i = 0; i < MAX_BOARD_TYPES; i++) {
      for (int j = 0; j < boardCount[i]; j++) {
        delete boards[i][j];
      }
      boardCount[i] = 0;
    }
  }

  void setScanned() {
    scanned = true;
  }

  bool isScanned() {
    return scanned;
  }

  PCF8574GPIOMultiplexer* GetGPIOMultiplexer(int index) {
    if (index < boardCount[BoardType::PCF8574_GPIO_Multiplexer]) {
      return static_cast<PCF8574GPIOMultiplexer*>(boards[BoardType::PCF8574_GPIO_Multiplexer][index]);
    } else {
      return nullptr;
    }
  }

  PCA9685ServoDriver* GetPCA9685ServoDriver(int index) {
    if (index < boardCount[BoardType::PCA9685_ServoDriver]) {
      return static_cast<PCA9685ServoDriver*>(boards[BoardType::PCA9685_ServoDriver][index]);
    } else {
      return nullptr;
    }
  }


private:
  Board* boards[MAX_BOARD_TYPES][MAX_DEVICES_PER_BOARD_TYPE];
  int boardCount[MAX_BOARD_TYPES];
  bool scanned = false;
};



I2CConfig scanI2CDevices() {
  I2CConfig i2cConfig;  // Create an instance of the I2CConfig class
  byte foundDevices = 0;
  // Scan for PCA9685 (0x40 to 0x7F)
  for (byte address = 0x40; address <= 0x7F; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("\nPCA9685 servo driver board found at address 0x");
      Serial.print(address, HEX);
      Serial.println();
      i2cConfig.addBoard(PCA9685_ServoDriver, address);  // Add to the I2CConfig instance
      foundDevices++;
    } else {
      Serial.print(".");
    }
  }

  // Scan for PCF8574 (0x20 to 0x27)
  for (byte address = 0x20; address <= 0x27; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("\nPCF8574 GPIO multiplexer found at address 0x");
      Serial.print(address, HEX);
      Serial.println();
      i2cConfig.addBoard(PCF8574_GPIO_Multiplexer, address);  // Add to the I2CConfig instance
      foundDevices++;
    } else {
      Serial.print(".");
    }
  }

  if (foundDevices == 0) {
    Serial.println("No devices found");
  } else {
    Serial.print("Found ");
    Serial.print(foundDevices);
    Serial.println(" device(s) on the I2C bus");
  }
  i2cConfig.setScanned();

  return i2cConfig;  // Return the I2CConfig instance
}




void processSerialInput(I2CConfig& i2cConfig) {
  static char inputBuffer[64];  // Buffer to hold the input string
  static byte index = 0;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer[index] = '\0';  // Null-terminate the input string
      index = 0;                  // Reset the index for the next command

      // Process the command using strtok
      char* command = strtok(inputBuffer, " ");
      if (command != NULL) {
        if (strcmp(command, "scan") == 0) {
          i2cConfig = scanI2CDevices();  // Perform scan and store the configuration
        } else if (strcmp(command, "print") == 0) {
          if (i2cConfig.isScanned()) {
            i2cConfig = scanI2CDevices();  // Perform scan if the configuration is not available
          }
          i2cConfig.printConfig();  // Print the configuration
        } else if (strcmp(command, "readPin") == 0) {
          // Parse the board index and pin number from the input using strtok
          char* boardIndexStr = strtok(NULL, " ");
          char* pinNumberStr = strtok(NULL, " ");
          if (boardIndexStr != NULL && pinNumberStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            int pinNumber = atoi(pinNumberStr);
            if (boardIndex >= 0 && pinNumber >= 0) {
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfig.GetGPIOMultiplexer(boardIndex);
              if (gpioMultiplexer) {
                bool pinState = gpioMultiplexer->readPin(pinNumber);
                Serial.print("GPIO Pin ");
                Serial.print(pinNumber);
                Serial.print(" on board index ");
                Serial.print(boardIndex);
                Serial.print(" is: ");
                Serial.println(pinState ? "HIGH" : "LOW");
              } else {
                Serial.println("Invalid board index.");
              }
            } else {
              Serial.println("Invalid input. Usage: readPin <boardIndex> <pinNumber>");
            }
          } else {
            Serial.println("Invalid input. Usage: readPin <boardIndex> <pinNumber>");
          }
        } else if (strcmp(command, "readPins") == 0) {
          char* boardIndexStr = strtok(NULL, " ");
          if (boardIndexStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            if (boardIndex >= 0) {
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfig.GetGPIOMultiplexer(boardIndex);
              if (gpioMultiplexer) {
                for (int pinNumber = 0; pinNumber < 8; pinNumber++) {
                  bool pinState = gpioMultiplexer->readPin(pinNumber);
                  Serial.print("GPIO Pin ");
                  Serial.print(pinNumber);
                  Serial.print(" on board index ");
                  Serial.print(boardIndex);
                  Serial.print(" is: ");
                  Serial.println(pinState ? "HIGH" : "LOW");
                }
              } else {
                Serial.println("Invalid board index.");
              }
            } else {
              Serial.println("Invalid input. Usage: readPins <boardIndex>");
            }
          } else {
            Serial.println("Invalid input. Usage: readPins <boardIndex>");
          }

        } else if (strcmp(command, "readPinsInLoop") == 0) {
          char* boardIndexStr = strtok(NULL, " ");
          char* secondsStr = strtok(NULL, " ");
          if (boardIndexStr != NULL && secondsStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            int seconds = atoi(secondsStr);
            if (boardIndex >= 0 && seconds >= 0) {
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfig.GetGPIOMultiplexer(boardIndex);
              if (gpioMultiplexer) {
                gpioMultiplexer->pcf8574Loop(seconds);
              } else {
                Serial.println("Invalid board index.");
              }
            } else {
              Serial.println("Invalid input. Usage: readPinsInLoop <boardIndex> <secondsToLoop>");
            }
          } else {
            Serial.println("Invalid input. Usage: readPinsInLoop <boardIndex> <secondsToLoop>");
          }
        } else {
          Serial.print("Unknown command:");
          Serial.println(inputBuffer);
          printHelp();
        }
      }
    } else {
      // Append the character to the input buffer
      inputBuffer[index++] = c;
      // Check for buffer overflow
      if (index >= sizeof(inputBuffer)) {
        index = 0;
      }
    }
  }
}






void printHelp() {
  Serial.println("The available commands are:");
  Serial.println("- scan: Scan for I2C devices");
  Serial.println("- print: Print the I2C bus configuration");
  Serial.println("- readPin <boardIndex> <pinNumber> : Reads a pin given the board index and pin number");
  Serial.println("- readPins <boardIndex> : Reads all pins given the board index");
  Serial.println("- readPinsInLoop <boardIndex> <seconds> : Loops reading pins for a number of seconds");
  Serial.println("- setServoAngle <boardIndex> <pinNumber> <angle>: Set the servo attached to a pin on a board to an angle");
}


I2CConfig i2cConfig; 

void setup() {
  Wire.begin();        // Initialize I2C bus
  Serial.begin(9600);  // Initialize serial communication
  delay(2000);         // Give some time for the serial monitor to open
  i2cConfig = scanI2CDevices();

  printHelp();
}


void loop() {
  processSerialInput(i2cConfig);
}
