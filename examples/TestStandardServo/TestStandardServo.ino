#include <standardServo.h>
#include "debugLog.h"

const int PIN_1 = 10;
StandardServo servo1(PIN_1);


void setup() {
  int baudRate = 9600;
  Serial.begin(baudRate);
  debugLog("Upload Date: ", __DATE__);
  debugLog("Upload Time: ", __TIME__);  
  servo1.begin();
  servo1.write(20);

}

void loop() {
  // put your main code here, to run repeatedly:

}
