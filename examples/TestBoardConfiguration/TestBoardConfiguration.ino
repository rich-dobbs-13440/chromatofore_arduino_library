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
  static String inputString = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      // End of command, process the input
      inputString.trim(); // Trim leading and trailing spaces
      if (inputString == "scanI2CDevices") {
        scanI2CDevices();
      } else {
        Serial.println("Unknown command. Type 'scanI2CDevices' and press Enter to scan for I2C devices.");
      }
      inputString = ""; // Clear the input string for the next command
    } else {
      // Append the character to the input string
      inputString += c;
    }
  }
}

void scanI2CDevices() {
  // The rest of the scanI2CDevices function remains unchanged
  // ...
}
