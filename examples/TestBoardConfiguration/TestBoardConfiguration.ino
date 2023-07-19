#include <Wire.h>

const int MAX_DEVICES_PER_BOARD = 5; // Maximum number of devices per board
const int MAX_BOARDS = 5; // Maximum number of boards

class I2CConfig {
public:
  I2CConfig() {
    reset();
  }

  void addBoard(int boardAddress, byte deviceAddress) {
    if (numBoards < MAX_BOARDS) {
      boards[numBoards++] = boardAddress;
      if (numDevicesPerBoard[boardAddress] < MAX_DEVICES_PER_BOARD) {
        devices[boardAddress][numDevicesPerBoard[boardAddress]++] = deviceAddress;
      }
    }
  }

  void printConfig() {
    Serial.println("I2C bus configuration:");
    for (int i = 0; i < numBoards; i++) {
      Serial.print("Board at address 0x");
      Serial.println(boards[i], HEX);
      Serial.println("Devices:");
      for (int j = 0; j < numDevicesPerBoard[boards[i]]; j++) {
        Serial.print("  - Device at address 0x");
        Serial.println(devices[boards[i]][j], HEX);
      }
      Serial.println();
    }
  }

  void reset() {
    numBoards = 0;
    for (int i = 0; i < MAX_BOARDS; i++) {
      numDevicesPerBoard[i] = 0;
      for (int j = 0; j < MAX_DEVICES_PER_BOARD; j++) {
        devices[i][j] = 0;
      }
    }
  }

  int getNumBoards() const {
    return numBoards;
  }  

private:
  int boards[MAX_BOARDS];
  byte devices[MAX_BOARDS][MAX_DEVICES_PER_BOARD];
  int numBoards;
  int numDevicesPerBoard[MAX_BOARDS];
};

I2CConfig i2cConfig;

I2CConfig scanI2CDevices();
void processSerialInput();

void setup() {
  Wire.begin(); // Initialize I2C bus
  Serial.begin(9600); // Initialize serial communication
  delay(2000); // Give some time for the serial monitor to open

  Serial.println("Type 'scanI2CDevices' and press Enter to scan for I2C devices.");
}

void loop() {
  processSerialInput();
  // Your other main code here (if any)
}
void processSerialInput() {
  static char inputBuffer[64]; // Buffer to hold the input string
  static byte index = 0;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer[index] = '\0'; // Null-terminate the input string
      index = 0; // Reset the index for the next command

      // Process the command using strtok
      char *command = strtok(inputBuffer, " ");
      if (command != NULL) {
        if (strcmp(command, "scanI2CDevices") == 0) {
          i2cConfig = scanI2CDevices(); // Perform scan and store the configuration
        } else if (strcmp(command, "printConfig") == 0) {
          if (i2cConfig.getNumBoards() == 0) {
            i2cConfig = scanI2CDevices(); // Perform scan if the configuration is not available
          }
          i2cConfig.printConfig(); // Print the configuration
        } else {
          Serial.println("Unknown command. The available commands are:");
          Serial.println("- scanI2CDevices: Scan for I2C devices");
          Serial.println("- printConfig: Print the I2C bus configuration");
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

I2CConfig scanI2CDevices() {
  I2CConfig i2cConfig; // Create an instance of the I2CConfig class

  byte foundDevices = 0;

  // Scan for PCA9685 (0x40 to 0x7F)
  for (byte address = 0x40; address <= 0x7F; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("\nPCA9685 servo driver board found at address 0x");
      Serial.print(address, HEX);
      Serial.println();
      i2cConfig.addBoard(0x40, address); // Add to the I2CConfig instance
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
      i2cConfig.addBoard(0x20, address); // Add to the I2CConfig instance
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

  return i2cConfig; // Return the I2CConfig instance
}

