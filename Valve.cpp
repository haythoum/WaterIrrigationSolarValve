#include "Valve.h"
extern "C" {
#include "stm32wlxx_hal.h"
}
extern RTC_HandleTypeDef hrtc;   // defined in RtcManager.cpp

// PLACEHOLDER — set to this board's actual relay wiring before flashing.
// PA6/PA7 are reserved (LoRa RF switch, see LoraService.cpp) — do not reuse.
#define VALVE_PIN PA8

// Dedicated backup register for this slave's valve state. Free to use —
// this project has no other persisted counters (unlike the master, which
// already uses DR0-DR8 for boot count / pump state / valve mask / etc.).
#define VALVE_BKP_REG RTC_BKP_DR0

void ValveManager::begin() {
  pinMode(VALVE_PIN, OUTPUT);

  uint32_t saved = HAL_RTCEx_BKUPRead(&hrtc, VALVE_BKP_REG);
  _open = (saved == 1);

  digitalWrite(VALVE_PIN, _open ? HIGH : LOW);
  if (_open) {
    Serial.println("[VALVE] Restored OPEN from before reboot");
  } else {
    Serial.println("[VALVE] Restored CLOSED (or no prior state) — safe default");
  }
}

void ValveManager::persist() {
  HAL_RTCEx_BKUPWrite(&hrtc, VALVE_BKP_REG, _open ? 1u : 0u);
}

void ValveManager::open() {
  digitalWrite(VALVE_PIN, HIGH);
  _open = true;
  persist();
  Serial.println("[VALVE] OPEN");
}

void ValveManager::close() {
  digitalWrite(VALVE_PIN, LOW);
  _open = false;
  persist();
  Serial.println("[VALVE] CLOSE");
}

bool ValveManager::isOpen() const {
  return _open;
}
