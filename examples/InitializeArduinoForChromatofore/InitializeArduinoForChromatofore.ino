
#include <chromatofore.h>
#include <debugLog.h>
#include <stdio.h> 

ChromatoforeFilamentChanger chromatoforeFilamentChanger;


void dumpEEPROMValues(const ChromatoforeFilamentChanger& changer) {
  int numTools = changer.getActuatorArraySize();

  // Print header
  Serial.println("Tool  │  Axes  ");
  Serial.println("      │  B         │   C        │   X");
  Serial.println("      │  Min   Max │   Min  Max │   Min  Max");
  Serial.println("──────┼────────────┼────────────┼───────────");


  // Iterate through all possible tools
  for (int tool = 0; tool < numTools; tool++) {
    // Retrieve the maximum values
    uint8_t maxB = changer.getMaximumAngleB(tool);
    uint8_t maxC = changer.getMaximumAngleC(tool);
    uint8_t maxX = changer.getMaximumAngleX(tool);

    // Retrieve the minimum values
    uint8_t minB = changer.getMinimumAngleB(tool);
    uint8_t minC = changer.getMinimumAngleC(tool);
    uint8_t minX = changer.getMinimumAngleX(tool);

    // Print the tool number and values for each axis
    char buffer[128];
    sprintf(buffer, " %2d  ", tool);
    Serial.print(buffer);
    sprintf(buffer, " │  %3d   %3d", minB, maxB);
    Serial.print(buffer);
    sprintf(buffer, " │  %3d   %3d", minC, maxC);
    Serial.print(buffer);
    sprintf(buffer, " │  %3d   %3d", minX, maxX);
    Serial.println(buffer);
  }
}



void setup() {
  chromatoforeFilamentChanger.begin();  // Needed to get debugLog working!
  chromatoforeFilamentChanger.initializeEEPROM();
  debugLog("sizeof(chromatoforeFilamentChanger)", sizeof(chromatoforeFilamentChanger));
  dumpEEPROMValues(chromatoforeFilamentChanger);

}

void loop() {
  // put your main code here, to run repeatedly:

}
