#pragma once
#include <Arduino.h>
#include "LoraService.h"
#include "Valve.h"
#include "RtcManager.h"

class Controller {
public:
  void begin();
  void run();
private:
  RtcManager rtc;
  LoraService lora;
  ValveManager valves;
  void handleCommand(const String& cmd);
};
