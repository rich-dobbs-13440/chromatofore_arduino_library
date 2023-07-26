#include "debugLog.h"


String asHex(byte byteValue) {
  char hex_string[5]; 
  sprintf(hex_string, "0x%02X", byteValue);
  return hex_string;
}