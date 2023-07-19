#include <Wire.h>

void scanI2CDevices();
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
          scanI2CDevices();
        } else {
          Serial.println("Unknown command. The grammar is as follows:");
          Serial.println("scanI2CDevices - Scan for I2C devices");
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

void scanI2CDevices() {
  Serial.println("Scanning I2C devices...");

  byte foundDevices = 0;

  // Scan for PCA9685 (0x40 to 0x7F)
  for (byte address = 0x40; address <= 0x7F; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("\nPCA9685 servo driver board found at address 0x");
      Serial.print(address, HEX);
      Serial.println();
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
}

