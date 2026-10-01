#include "RtcManager.h"
extern "C" {
#include "stm32wlxx_hal.h"
}

RTC_HandleTypeDef hrtc;  // real definition — backup-register calls need this

void RtcManager::begin() {
  __HAL_RCC_RTCAPB_CLK_ENABLE();   // required — see comment in RtcManager.h
  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_LSI_ENABLE();
  while (!__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY));

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Serial.println("[RTC] clk cfg FAILED");
  }
  __HAL_RCC_RTC_ENABLE();

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 249;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) {
    Serial.println("[RTC] init FAILED — valve state persistence will not work");
  }
}
