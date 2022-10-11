#include <Arduino.h>
#include <string>
#include <Preferences.h>
#include <stdio.h>
#include "RAK4270/RAK4270.h"
#include "lut-reader/lut-reader.h"
#include "soil-util/soildriver.h"

#define PWR_PIN 27
#define STAT_PIN 26

#define RAK_TX 16// third pin from top-right of BG 96
#define RAK_RX 17// fourth pin from top-right of BG 96
#define MODEPIN 21
#define RS485_RI 19
#define RS485_RO 18

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  900       /* Time ESP32 will go to sleep (in seconds) */
#define SURFACE_TO_PROBE  0.809       

using namespace std;

RTC_DATA_ATTR int bootCount = 0;

HardwareSerial RakSerial(1);
// HardwareSerial SoilSerial(2);
RAK4270 RAK;
// LUT lut;
int fetchRetryCount = 0;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

String bytetoStr(byte a);
