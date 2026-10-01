#include "LoraService.h"
#include "Config.h"

STM32WLx radio = new STM32WLx_Module();
volatile bool LoraService::_receivedFlag = false;

// Confirmed from Ebyte's E77-900M22S product page: PA6 = RF_TXEN, PA7 = RF_RXEN.
// TX = TXEN HIGH / RXEN LOW. RX = TXEN LOW / RXEN HIGH.
// Each row below must have RFSWITCH_MAX_PINS (5) entries to match
// rfswitch_pins[], even though only the first two slots are actually wired
// on this module — the rest are padded with RADIOLIB_NC / LOW.
static const uint32_t rfswitch_pins[] = {PA7, PA6, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
  {STM32WLx::MODE_IDLE,   {LOW,  LOW,  LOW, LOW, LOW}},
  {STM32WLx::MODE_RX,     {HIGH, LOW,  LOW, LOW, LOW}},
  {STM32WLx::MODE_TX_LP,  {LOW,  HIGH, LOW, LOW, LOW}},
  {STM32WLx::MODE_TX_HP,  {LOW,  HIGH, LOW, LOW, LOW}},  // same external state as LP — PA choice is internal to the chip
  END_OF_MODE_TABLE,
};

bool LoraService::begin(float frequencyMHz) {
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  ConfigLoRa_t config;
  config.frequency = frequencyMHz;
  radio.tcxoVoltage = LORA_TCXO_VOLTAGE;

  int state = radio.begin(config);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[LORA] begin failed, code "); Serial.println(state);
    return false;
  }

  radio.setDio1Action(setFlag);
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[LORA] startReceive failed, code "); Serial.println(state);
    return false;
  }
  Serial.println("[LORA] Ready");
  return true;
}

void LoraService::setFlag() {
  _receivedFlag = true;
}

bool LoraService::send(const String& payload) {
  String p = payload;   // RadioLib's transmit() wants a non-const String&
  int state = radio.transmit(p);
  radio.startReceive();   // always return to listening after a TX
  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("[LORA] Sent: "); Serial.println(p);
    return true;
  }
  Serial.print("[LORA] Send failed, code "); Serial.println(state);
  return false;
}

bool LoraService::available() {
  return _receivedFlag;
}

String LoraService::pull() {
  _receivedFlag = false;
  String str;
  int state = radio.readData(str);
  radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) return str;
  if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println("[LORA] CRC mismatch — packet dropped");
  } else {
    Serial.print("[LORA] Read error, code "); Serial.println(state);
  }
  return "";
}

void LoraService::task() {
  // Interrupt-driven — nothing to poll. Present for symmetry with the
  // rest of this project's services (SimManager::task(), etc.).
}
