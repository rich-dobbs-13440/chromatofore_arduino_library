#pragma once

#include <Serial.h>

static const char* logPrefix = "-- debug --";

template <typename T, typename... Args>
void debugLog(T first, Args... args) {
  Serial.print(logPrefix);
  Serial.print(" ");
  Serial.print(first);
  ((Serial.print(" "), Serial.print(args)), ...);
  Serial.println();
  Serial.flush();
}

String asHex(byte byteValue); 

