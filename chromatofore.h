#pragma once

#include <Arduino.h>
#include <earwig.h>
#include <string.h>



class ChromatoforeFilamentChanger {
 private:
  String version = "0.1g";
  EarwigFilamentActuator** actuatorArray;
  int actuatorArraySize;
  int baudRate = 9600;
  static const int BUFFER_SIZE = 256;  // Size of the input buffer
  char inputBuffer[BUFFER_SIZE];       // Input buffer to store characters
  int bufferIndex = 0;       // Index to keep track of the buffer position
  int currentFilament = -1;  // None selected
  int nextFilament = -1;     // None selected

  void handleSerial();
  void processInputBuffer();
  void acknowledgeCommand(const String& command);
  byte calculateChecksum(const String& command);

 public:
  ChromatoforeFilamentChanger(int size = 64);
  ~ChromatoforeFilamentChanger();

  void begin();
  void loop();

  void addActuator(int index, EarwigFilamentActuator* actuator);
  void removeActuator(int index);
  EarwigFilamentActuator* getActuator(int index);
  void selectNextFilament(int index) { nextFilament = index; }
  void setCurrentFilament(int index) { currentFilament = index; }
};
