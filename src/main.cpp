// #include <WiFi.h>
// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>
#include <Arduino.h>
#include <iostream>
#include <string>
#include <Preferences.h>
#include<stdio.h>
#include <RAK4270/RAK4270.h>

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600      // Time ESP32 will go to sleep (in seconds)

RTC_DATA_ATTR int bootCount = 0;
char* DevEui = {0,};
char* AppEui = {0,};


Preferences prfs;

// hardwareserial count from 0:
// 0 is reserved for typical TX RX
// 1 is for send at command

// hardwareserial 1, 16--TX1, 17 --RX1  worked
// hardwareserial 1, 26--TX1, 27 --RX1  worked
// hardwareserial 2, 26--TX1, 27 --RX1 worked

const char *ssid = "DevOps";
const char *password = "nodemcu01";
HardwareSerial RakSerial(1);
RAK4270 RAK;

void setup()
{
  RAK.init(&RakSerial, 16,17);
  // rx, tx
  Serial.begin(115200);
  Serial.println("Booting");

  prfs.begin("lora-prfs", false);

   DevEui = RAK.getDevEui();

  if (!prfs.getUInt("init"))
  {
    Serial.println("It seems inital boot. matching DevEUI and AppEUI...");
    unsigned long t1 = millis();
    unsigned long t2 = millis();


    RAK.setAppEui(DevEui);
    unsigned long t3 = millis();

    Serial.println(String(t2 - t1) + "ms for getting deveui");
    Serial.println(String(t3 - t2) + "ms for matching deveui");
    RAK.restart();
    delay(2000);
    ESP.restart();
    prfs.putUInt("init", 1);
  }
  else
  {
    Serial.println("typical boot.");
  }


  for(int i = 0; i<16 ; i++){
    Serial.print(DevEui[i]);
  }
      Serial.println("");


  AppEui = RAK.getAppEui();
  Serial.print("AppEUI : ");
  for(int i = 0; i<16 ; i++){
    Serial.print(AppEui[i]);
  }
      Serial.println("");

  // newjoinLora();
  // newSendData("1", "AA");

  // joinLora();
  // delay(5000);

  // sendData("1234657");
  // delay(5000);


  // Set timer to 5 seconds
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("ESP gets to sleep " + String(TIME_TO_SLEEP) +
                 " Seconds");

  // Go to sleep now
  esp_deep_sleep_start();
}

void loop()
{
  delay(1000);
}