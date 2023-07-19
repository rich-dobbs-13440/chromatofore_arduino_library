#include <Wire.h>

void scanI2CDevices();

void setup() {
  Wire.begin(); // Initialize I2C bus
  Serial.begin(9600); // Initialize serial communication
  delay(2000); // Give some time for the serial monitor to open

  // Call the function to scan for I2C devices
  scanI2CDevices();
}

void loop() {
  // Your main code here (if any)
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
