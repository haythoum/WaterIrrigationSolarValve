#pragma once
#include <Arduino.h>
#include "Config.h"

// Exactly ONE valve per slave under the compact S<id><action> protocol.
// State persists across reboots via one RTC backup register, mirroring
// the master's own local-valve persistence (persistRunState() /
// restoreRunStateIfSafe()).
class ValveManager {
public:
  void begin();   // restores last persisted state before returning
  void open();
  void close();
  bool isOpen() const;
private:
  bool _open = false;
  void persist();
};
