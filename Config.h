#pragma once

// ---- LoRa (internal SubGHz radio, STM32WLE5CC / Ebyte E77-900M22S) ----
#define LORA_FREQUENCY_MHZ   868.0f
#define LORA_TCXO_VOLTAGE    1.7f   // set to whatever was confirmed working on this hardware

// ---- This slave's identity ----
// This slave owns exactly ONE global valve number, shared with the
// master's own numbering (Section 11 of the spec — global numbering,
// local = 1..localValveCount on the master, remote = everything above
// that). The master addresses this board simply as "V<OWNED_VALVE>",
// e.g. OWNED_VALVE=3 -> "V3O" opens, "V3C" closes, "V3S" queries,
// "V3R" resets this board.
#define OWNED_VALVE 3
