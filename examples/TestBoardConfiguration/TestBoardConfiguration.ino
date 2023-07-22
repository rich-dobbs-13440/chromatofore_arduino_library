/* 

This sketch provides a serial command line interface useful for identifying
the I2C board configuration and testing out the functionality of sensors
and servos attached to the daughter boards.

The immediate use will be for building the Earwig actuators and testing the 
associated filament sensors.

*/


#include <Wire.h>
#include <i2cConfiguration.h>


I2CConfiguration i2cConfiguration;


void printHelp() {
  Serial.println("The available commands are:");
  Serial.println("- scan: Scan for I2C devices.");
  Serial.println("- print: Print the I2C bus configuration.");
  Serial.println("- readPin <boardIndex> <pinNumber> : Reads a GPIO pin attached to a PCF8574 board.");
  Serial.println("- readPins <boardIndex> : Reads all GPIO pin attached to a PCF8574 board.");
  Serial.println("- readPinsInLoop <boardIndex> <seconds> : Loops reading pins for a number of seconds.");
  Serial.println("- setServoAngle <boardIndex> <pinNumber> <angle>: Controls a servo attached to a PCA9685 board.");
}


void processSerialInput(I2CConfiguration& i2cConfiguration) {
  static char inputBuffer[64];  // Buffer to hold the input string
  static byte index = 0;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer[index] = '\0';  // Null-terminate the input string
      Serial.print("$ ");
      Serial.println(inputBuffer);
      index = 0;                  // Reset the index for the next command

      // Process the command using strtok
      char* command = strtok(inputBuffer, " ");
      if (command != NULL) {
        if (strcmp(command, "scan") == 0) {
          i2cConfiguration.scan();  // Perform scan and store the configuration
        } else if (strcmp(command, "print") == 0) {
          if (!i2cConfiguration.isScanned()) {
            i2cConfiguration.scan();  // Perform scan if the configuration is not available
          }
          i2cConfiguration.printConfig();  // Print the configuration
        } else if (strcmp(command, "readPin") == 0) {
          if (!i2cConfiguration.isScanned()) {
            i2cConfiguration.scan();  // Perform scan if the configuration is not available
          }          
          // Parse the board index and pin number from the input using strtok
          char* boardIndexStr = strtok(NULL, " ");
          char* pinNumberStr = strtok(NULL, " ");
          if (boardIndexStr != NULL && pinNumberStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            int pinNumber = atoi(pinNumberStr);
            if (boardIndex >= 0 && pinNumber >= 0) {
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfiguration.GetGPIOMultiplexer(boardIndex);
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
          if (!i2cConfiguration.isScanned()) {
            i2cConfiguration.scan();  // Perform scan if the configuration is not available
          }          
          char* boardIndexStr = strtok(NULL, " ");
          if (boardIndexStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            if (boardIndex >= 0) {
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfiguration.GetGPIOMultiplexer(boardIndex);
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
              PCF8574GPIOMultiplexer* gpioMultiplexer = i2cConfiguration.GetGPIOMultiplexer(boardIndex);
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

        } else if (strcmp(command, "setServoAngle") == 0) {
          
          // Parse the board index and pin number from the input using strtok
          char* boardIndexStr = strtok(NULL, " ");
          char* pinNumberStr = strtok(NULL, " ");
          char* angleStr = strtok(NULL, " ");
          if (boardIndexStr != NULL && pinNumberStr != NULL && angleStr != NULL) {
            int boardIndex = atoi(boardIndexStr);
            int pinNumber = atoi(pinNumberStr);
            int angle = atoi(angleStr);
            if (boardIndex >= 0 && pinNumber >= 0 && angle >= 0) {
              PCA9685ServoDriver* servoDriver = i2cConfiguration.GetPCA9685ServoDriver(boardIndex);
              if (servoDriver) {
                if (!servoDriver->setServoAngle(pinNumber, angle)) {
                  Serial.println("Invalid pin index or angle.");
                }
              } else {
                Serial.println("Invalid board index.");
              } 
            } else {
              Serial.println("Invalid input. Usage: setServoAngle <boardIndex> <pinNumber> <angle>");
            }   
          } else {
            Serial.println("Invalid input. Usage: setServoAngle <boardIndex> <pinNumber> <angle>");
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


void setup() {
  Serial.begin(9600);  // Initialize serial communication
  Wire.setTimeout(200);
  Wire.begin();  // Initialize I2C bus  
  Wire.setTimeout(200);
  delay(2000);         // Give some time for the serial monitor to open
  i2cConfiguration.scan();
  printHelp();
}


void loop() {
  processSerialInput(i2cConfiguration);
}
