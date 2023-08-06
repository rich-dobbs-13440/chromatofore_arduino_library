/* The axes used for the machine for the Earwig Filament Actuator are:

      Rotation of fixed clamp:  B
      Rotation of moving clamp: C
      Location of moving clamp: X

Note that at this time, actual location of the moving clamp is not linear with
the X, but is a function of the linkage kinematics.

The following GCODE parsing is currently implemented:


G1 T0 B10 ; For actuator 0, move the fixed clamp servo to an angle of 10 degrees

G1 T1 C10 ; For actuator 1, Move the clamp servo to an angle of 10 degrees

G1 T2 X10 ; For actuator 2, move the pusher servo to an angle of 10 degrees

G10 T3 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles
for tool 3

G10 T0 L1 B101 C101 X145 ; Remember maximum values for the B, C, and X axis
angles for tool 0

G28 T1 B C X ; Home all axes, say for loading or unloading filament.



G1 T1 E10 F10 ; Extrude 10 mm of filament, feed rate currently ignore.  

G1 T1 E-100 Q1 ;  Retract 100 mm of filament, but only so long as filament is detected.

M119 T0; Report the switch state - for the filament detector and endstops.

M999 T1 B1 C1 X1:  Reset the servo associated with a particular axes, presumably after fixing a problem.

*/



#include "iSerialHandler.h"
#include "chromatofore.h"


struct GcodeCommand {
  float values[26]; // 26 for each letter parameter from A to Z.

  // The values should be initialized to NaN.
  GcodeCommand() {
    for (int i = 0; i < 26; i++) {
      values[i] = std::numeric_limits<float>::quiet_NaN();
    }
  }

  // Helper function to set values by letter.
  void set(char letter, float value) {
    letter = toupper(letter);
    int index = letter - 'A'; // 'A' is 0, 'B' is 1, etc.
    if (index >= 0 && index < 26) {
      values[index] = value;
    }
  }

  // Helper function to get values by letter.
  float get(char letter) const {
    int index = letter - 'A';
    if (index >= 0 && index < 26) {
      return values[index];
    }
    return std::numeric_limits<float>::quiet_NaN();
  }

  bool isSet(char letter) const {
    return !std::isnan(get(letter));
  }
};

class GcodeCommandBuffer {
 public:
  GcodeCommandBuffer(int buffer_size)
      : buffer_size(buffer_size),
        gcode_buffer(new GcodeCommand[buffer_size]()),
        head(0),
        tail(0),
        current_size(0) {}

  ~GcodeCommandBuffer() {
    delete[] gcode_buffer;
  }

  bool put(const GcodeCommand& command) {
    if (current_size >= buffer_size) {
      return false; // Buffer overflow.
    }
    gcode_buffer[tail] = command;
    tail = (tail + 1) % buffer_size;
    current_size++;
    return true; // Successfully added to buffer.
  }

  GcodeCommand get() {
    if (current_size == 0) {
      // Buffer underflow, return a null command (all NANs)
      return GcodeCommand();
    }
    GcodeCommand command = gcode_buffer[head];
    head = (head + 1) % buffer_size;
    current_size--;
    return command;
  }

  bool isEmpty() const {
    return current_size == 0;
  }

 private:
  int buffer_size;
  GcodeCommand* gcode_buffer;
  int head;
  int tail;
  int current_size;
};



class GcodeSerialHandler : public ISerialHandler {
 public:
  GcodeSerialHandler(int gcode_buffer_size)
      : commandBuffer(gcode_buffer_size) {}

  void handleSerial() override;
  void initialize(ChromatoforeFilamentChanger& changer) {
    this->changer = &changer;
  }

  GcodeCommand parseGcodeLine(const char *format, ...);
  void executeGcodeCommand(const GcodeCommand& cmd); 

 private:
  GcodeCommand processInputBuffer();
  
  void acknowledgeCommand(const String &command);
  byte calculateChecksum(const String &command);

  int bufferIndex = 0;  // Index to keep track of the buffer position
  bool echoCharacters = false;
  static const int BUFFER_SIZE = 256;  // Size of the input buffer
  char inputBuffer[BUFFER_SIZE];       // Input buffer to store characters

  ChromatoforeFilamentChanger* changer = nullptr;
  bool debug = false;

  GcodeCommandBuffer commandBuffer;
};