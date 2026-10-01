#pragma once
#include <RadioLib.h>

// Thin wrapper around RadioLib's STM32WLx driver for the WLE5's internal
// SubGHz radio. Same class used on the main (sender) board — kept here as
// its own pair of files so this project stays self-contained.
class LoraService {
public:
  bool begin(float frequencyMHz);
  bool send(const String& payload);
  bool available();
  String pull();
  void task();   // interrupt-driven; kept for symmetry with other services

private:
  static void setFlag();
  static volatile bool _receivedFlag;
};
