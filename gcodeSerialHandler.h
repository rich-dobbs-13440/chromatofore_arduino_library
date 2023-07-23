#include "iSerialHandler.h"
#include "chromatofore.h"

//ISerialHandler* getGcodeSerialHandler(ChromatoforeFilamentChanger& changer);

class GcodeSerialHandler : public ISerialHandler {
 public:
  void handleSerial() override;
  void initialize(ChromatoforeFilamentChanger& changer) {
    this->changer = &changer;
  }

 private:
  void processInputBuffer();
  void acknowledgeCommand(const String &command);
  byte calculateChecksum(const String &command);

  int bufferIndex = 0;  // Index to keep track of the buffer position
  bool echoCharacters = false;
  static const int BUFFER_SIZE = 256;  // Size of the input buffer
  char inputBuffer[BUFFER_SIZE];       // Input buffer to store characters

  ChromatoforeFilamentChanger* changer = nullptr;
  bool debug = false;
};