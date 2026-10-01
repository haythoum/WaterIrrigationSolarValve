#include "Controller.h"
#include "Config.h"

void Controller::begin() {
  Serial.println("[BOOT] LoRa Valve Receiver");
  Serial.print("[BOOT] OWNED_VALVE="); Serial.println(OWNED_VALVE);
  rtc.begin();       // must run BEFORE valves.begin() — backup registers need this init first
  valves.begin();    // restores persisted open/closed state here
  if (!lora.begin(LORA_FREQUENCY_MHZ)) {
    Serial.println("[FATAL] LoRa init failed — check wiring / TCXO voltage");
  }
}

void Controller::run() {
  lora.task();
  if (lora.available()) {
    String cmd = lora.pull();
    if (cmd.length() > 0) {
      Serial.print("[RX] "); Serial.println(cmd);
      handleCommand(cmd);
    }
  }
}

// Wire format:  V<globalId><action>   action = 'O' open / 'C' close / 'S' status / 'R' reset
// Reply format: A<globalId><state>    state  = 'O' open / 'C' closed / 'R' (reset acked, about to reboot)
//
// A packet not addressed to this board's OWNED_VALVE, or that isn't
// shaped like a command at all (e.g. it's an A-reply, ours or another
// slave's), is silently ignored — no reply sent. This is what lets
// several slaves share one link with no collisions, and what stops a
// slave from ever reacting to its own transmitted ack (a reply is never
// mistaken for a command since it's the SAME 'V'-only check that filters
// it — an "A..." reply never starts with 'V' at all).
void Controller::handleCommand(const String& cmdIn) {
  String cmd = cmdIn;
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.length() != 3 || cmd.charAt(0) != 'V') return;

  int globalId = cmd.substring(1, 2).toInt();
  char action = cmd.charAt(2);
  if (action != 'O' && action != 'C' && action != 'S' && action != 'R') return;
  if (globalId != OWNED_VALVE) return;   // not addressed to us

  if (action == 'R') {
    lora.send("A" + String(globalId) + "R");
    Serial.println("[CMD] Reset requested — rebooting");
    Serial.flush();
    delay(300);
    NVIC_SystemReset();
    return;
  }

  bool nowOpen;
  if (action == 'O')      { valves.open();  nowOpen = true; }
  else if (action == 'C') { valves.close(); nowOpen = false; }
  else                     nowOpen = valves.isOpen();   // 'S' — query only, no side effect

  lora.send("A" + String(globalId) + (nowOpen ? "O" : "C"));
}
