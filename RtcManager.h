#pragma once
#include <Arduino.h>

// Minimal RTC setup — this slave never sleeps and never uses the calendar,
// it only needs the backup registers (for valve-state persistence across
// reboots) to be reliably accessible. Mirrors the init sequence proven
// necessary on the master board (RTCAPB clock enable was the fix for a
// real "[RTC] init FAILED" issue there — skipping it silently makes
// backup-register reads/writes unreliable, not obviously broken).
class RtcManager {
public:
  void begin();
};
