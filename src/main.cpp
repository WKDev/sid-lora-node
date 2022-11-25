//WATERLEVEL


#include "main.h"

#define DEBUG
#define WATERLEVEL
// #define LUT18
#define ESMUS10A
// #define SOIL
#define init_set_zerolevel

#ifdef SOIL
SoilDriver soil;
SoftwareSerial SoilSerial(19, 18);

void SoilProcess()
{
  byte layer = 1;
  while (true)
  {
    String ss = "";
    byte recv[21] = {
        1,
    };

    soil.getData(layer, recv);

#ifdef DEBUG
    Serial.print(" recv : ");

    for (int i = 0; i < 21; i++)
    {
      Serial.print(recv[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    float temp = ((recv[3] << 8) | recv[4]) / 10.0;
    float humid = ((recv[5] << 8) | recv[6]) / 10.0;
    int ec = (recv[7] << 8) | recv[8];
    float ph = ((recv[9] << 8) | recv[10]) / 100.0;
    int N = (recv[11] << 8) | recv[12];
    int P = (recv[13] << 8) | recv[14];
    int K = (recv[15] << 8) | recv[16];
    int salt = ((recv[17] << 8) | recv[18]);
    // int crc = (recv[19] << 8) | recv[20];

    // Serial.printf("layer : %d | temp : %1fC | humid : %1f% | ec : %d uS | tds : %d | N : %d | P : %d | K : %d | salt : %d \n", layer, temp, moisture, ec, tds, n, p, k, salt);

    Serial.printf("temp : %f | humid : %f | ec : %d uS | ph : %f | N : %d | P : %d | K : %d | salt : %d \n", temp, humid, ec, ph, N, P, K, salt);

#endif

    // 5번 시도하면 끝
    if (fetchRetryCount > 5)
    {

      Serial.println("fetching data failed 5 times. sending null data and gets to sleep");
      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();
      delay(500);
      RAK.restart();

      delay(1500);
      RAK.setJoin();
      delay(5000);
      // ss.c_str())
      String s_err = "000000000000000000000000000000000000000000";
      RAK.SendData("15", s_err.c_str());

      break;
    }

    if ((recv[0] == 0x01) && (recv[1] == 0x03) && (recv[2] == 0x10))
    {
      // String으로 변환
      for (int i = 3; i < 19; i++)
      {
        ss += bytetoStr(recv[i]);
      }
      delay(2000);

      Serial.println("");

      // RAK4270으로 전송
      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();

      Serial.println("RAK restart : ");

      RAK.restart();

      delay(1500);

      RAK.setJoin();
      delay(5000);
      // ss.c_str())

      // String fake = "008400ca00c802b2000f001500350105";

      // RAK.SendData("15", fake.c_str());

      RAK.SendData("15", ss.c_str());
      break;
    }

    else
    {
      Serial.println("fetching data failed..retrying");
      fetchRetryCount++;
      delay(1000);
    }
  }
}
#endif

#ifdef WATERLEVEL
LUT lut;

// 버튼 상태 관할 변수
unsigned long btnPressTime = 0;
unsigned long btnReleaseTime = 0;
int currentState = 0;
int lastState = 0;
int btnPressCount = 0;

// 영점이 어느 지점인지,
float zero_level_coefficient = 0;

void WaterlevelProcess()
{
  lut.init();

  while (true)
  {
    // float to hex
    String s = "00000000";
    byte ret[9] = {
        0x00,
    };

    // 5번 시도하면 끝
    if (fetchRetryCount > 5)
    {

      Serial.println("fetching data failed 5 times. sending null data and gets to sleep");
      fetchRetryCount = 0;

      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();
      delay(500);
      RAK.restart();

      delay(1500);
      RAK.setJoin();
      delay(5000);
      // ss.c_str())
      String s_err = "000000000000000000000000000000000000000000";
      RAK.SendData("15", s_err.c_str());

      break;
    }

    byte arres[4] = {
        0x00,
    };

    float waterlevel = lut.getLevel(ret); // returns waterlevel with m unit.
    float corrected_level = (zero_level_coefficient - waterlevel);

    if (corrected_level < 0.0)
    {
      corrected_level = 0;
      Serial.println("[WARN] Corrected Level is below 0,  raw: " + String(waterlevel) + "corrected : " + String(corrected_level));
    }

    Serial.print("\nret:");
    for (int i = 0; i < 9; i++)
    {
      Serial.print(ret[i], HEX);

      Serial.print(",");
    }

    if ((ret[0] == 0x01) && (ret[1] == 0x03)) // LUT-18
    {
      Serial.println();
      for (int i = 3; i > -1; i--)
      {
        arres[i] = *((char *)&corrected_level + i);
        s += bytetoStr(arres[i]);
      }

#ifdef DEBUG
      Serial.print("\nret:");
      for (int i = 0; i < 9; i++)
      {
        Serial.print(ret[i], HEX);

        Serial.print(",");
      }

      String ss;
      unsigned long hex_value;
      if (ret[2] == 0x04)
      { // LUT18
        Serial.println("LUT18");

        ss = bytetoStr(ret[3]) + bytetoStr(ret[4]) + bytetoStr(ret[5]) + bytetoStr(ret[6]);
        unsigned long hex_value = arres[3] << 24 | arres[2] << 16 | arres[1] << 8 | arres[0];
        float modified = *((float *)&hex_value);

        Serial.print("expected correctedvalue : ");
        Serial.println(corrected_level);
        Serial.print("typical(ss, raw) : ");
        Serial.println(ss);
        Serial.print("modified(s) : ");
        Serial.println(s);
        Serial.println(modified);
      }

      else if (ret[2] == 0x02)
      { // ESMUS10A
        Serial.println("ESMUS10A");
        ss = bytetoStr(ret[3]) + bytetoStr(ret[4]);
        unsigned long hex_value = arres[3] << 24 | arres[2] << 16 | arres[1] << 8 | arres[0];

        int modified = *((int *)&hex_value);

        Serial.print("expected correctedvalue : ");
        Serial.println(corrected_level);
        Serial.print("typical(ss, raw) : ");
        Serial.println(ss);
        Serial.print("modified(s) : ");
        Serial.println(s);
        Serial.println(modified);
      }
      else
      {
        fetchRetryCount++;
        delay(1000);
        continue;
      }
#endif

      String consoleStr = "raw : " + String(waterlevel) + " | corrected : " + String(corrected_level);
      Serial.println(consoleStr);
      // RAK4270을 통해 데이터 송신

      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();

      RAK.restart();

      

      delay(1500);
      RAK.setJoin();
      delay(5000);
      RAK.SendData("15", s.c_str());
      break;
    }
    else
    {
      fetchRetryCount++;
      delay(1000);
    }
  }
}
#endif

void setup()
{
  
  Serial.begin(115200);
  Serial.println("bootCount : " + String(bootCount));

  if (bootCount > 50)
  {
    bootCount = 0;
    Serial.println("bootCount reaches 50 times. system restarted for stability");
    ESP.restart();
  }

#ifdef WATERLEVEL
  lut.init();

  pinMode(25, INPUT);
  prfs.begin("ulwn", false);

  if (isnan(prfs.getFloat("zerolevel")))
  {
    prfs.putFloat("zerolevel", 0.0);
    zero_level_coefficient = 0;
    Serial.println("[ERROR] nan is stored in Preferences. coeff has been set to 0.");
  }
  else
  {
    zero_level_coefficient = prfs.getFloat("zerolevel");
  }

  Serial.println("stored offset is : " + String(zero_level_coefficient, 4U));

  byte recv[9] = {
      0x00,
  };

  int retryCount = 0;
  float init_level = 0.0;
  /////////////초기값 받아오기 
  while (true)
  {
    // 5번 시도하면 끝
    if (fetchRetryCount > 5)
    {
      Serial.println("fetching data failed 5 times. sending null data and gets to sleep");
      fetchRetryCount = 0;

      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();
      delay(500);
      RAK.restart();

      delay(1500);
      RAK.setJoin();
      delay(5000);
      // ss.c_str())
      String s_err = "000000000000000000000000000000000000000000";
      RAK.SendData("15", s_err.c_str());
      break;
    }

    // byte recv[9] = {
    //     0x00,
    // };
    byte arres[4] = {
        0x00,
    };
    
    init_level = lut.getLevel(recv); // returns waterlevel with m unit.

    Serial.print("\nrecv:");
    for (int i = 0; i < 9; i++)
    {
      Serial.print(recv[i], HEX);

      Serial.print(",");
    }
    
    if ((recv[0] == 0x01) && (recv[1] == 0x03) && ((recv[2] == 0x02) || (recv[2] == 0x04)))
    {
      Serial.println("Measuring init value was success");
      break;
    }
    else
    {
      Serial.println("Measuring init level was failed. retrying...");

      if(retryCount > 10){
        Serial.println("Measuring init level was failed. getting in to sleep...");
        esp_deep_sleep_start();

      }
      retryCount++;
      delay(500);
    }
  }

#ifdef init_set_zerolevel
  unsigned long millis_entry = millis();
  while (millis() < 10000)
  {
    // Serial.println(millis() % 1000);
    if ((millis() - millis_entry) == 1000)
    {
      Serial.println(String(10 - (millis() / 1000)) + "sec left");
      millis_entry = millis();
    }
    currentState = digitalRead(25);

    if (lastState == 0 && currentState == 1)
    {
      btnPressTime = millis();
      // Serial.println("press detected");
      lastState = 1;
    }
    else if (lastState == 1 && currentState == 0)
    {
      btnReleaseTime = millis();
      // Serial.println("release detected");
      lastState = 0;

      long pressDuration = btnReleaseTime - btnPressTime;
      // Serial.println(pressDuration);

      if (0 <= pressDuration && pressDuration < 500)
      {

        btnPressCount++;

        Serial.println("offset is setted : " + String(zero_level_coefficient + float(btnPressCount * 0.001), 4U));
        delay(50);
      }

      else if (pressDuration > 900)
      {
        prfs.putFloat("zerolevel", init_level);
        Serial.println("offset is setted : " + String(init_level, 4U));
        delay(2000);

        ESP.restart();
      }
    }
  }
  if (btnPressCount > 0)
  {
    float res = zero_level_coefficient + float(btnPressCount * 0.005);
    prfs.putFloat("zerolevel", res);
    Serial.println("coeff is setted : " + String(res, 4U));
    ESP.restart();
  }
#endif

    String s = "00000000";

        byte arres[4] = {
        0x00,
    };
    
    //  prfs.putFloat("zerolevel", 0.600);
    float waterlevel = init_level;
    float corrected_level = (zero_level_coefficient - waterlevel);

    if (corrected_level < 0.0)
    {
      corrected_level = 0;
      Serial.println("[WARN] Corrected Level is below 0,  raw: " + String(waterlevel, 4U) + "corrected : " + String(corrected_level,4U));
    }

    Serial.print("\nret:");
    for (int i = 0; i < 9; i++)
    {
      Serial.print(recv[i], HEX);

      Serial.print(",");
    }

    if ((recv[0] == 0x01) && (recv[1] == 0x03)) // LUT-18
    {
      Serial.println();
      for (int i = 3; i > -1; i--)
      {
        arres[i] = *((char *)&corrected_level + i);
        s += bytetoStr(arres[i]);
      }

#ifdef DEBUG
      Serial.print("\nret:");
      for (int i = 0; i < 9; i++)
      {
        Serial.print(recv[i], HEX);

        Serial.print(",");
      }

      String ss;
      unsigned long hex_value;
      if (recv[2] == 0x04)
      { // LUT18
        Serial.println("LUT18");

        ss = bytetoStr(recv[3]) + bytetoStr(recv[4]) + bytetoStr(recv[5]) + bytetoStr(recv[6]);
        unsigned long hex_value = arres[3] << 24 | arres[2] << 16 | arres[1] << 8 | arres[0];
        float modified = *((float *)&hex_value);

        Serial.print("expected correctedvalue : ");
        Serial.println(corrected_level,4);
        Serial.print("typical(ss, raw) : ");
        Serial.println(ss);
        Serial.print("modified(s) : ");
        Serial.println(s);
        Serial.println(modified,4);
      }

      else if (recv[2] == 0x02)
      { // ESMUS10A
        Serial.println("ESMUS10A");
        ss = bytetoStr(recv[3]) + bytetoStr(recv[4]);
        unsigned long hex_value = arres[3] << 24 | arres[2] << 16 | arres[1] << 8 | arres[0];

        int modified = *((int *)&hex_value);

        Serial.print("expected correctedvalue : ");
        Serial.println(corrected_level);
        Serial.print("typical(ss, raw) : ");
        Serial.println(ss);
        Serial.print("modified(s) : ");
        Serial.println(s);
        Serial.println(modified,4U);
      }
#endif

      String consoleStr = "raw : " + String(waterlevel) + " | corrected : " + String(corrected_level);
      Serial.println(consoleStr);
      // RAK4270을 통해 데이터 송신

      RAK.init(&RakSerial, RAK_TX, RAK_RX);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      RAK.wakeup();

      // RAK.restart();

      delay(1500);
      RAK.setJoin();
      delay(5000);
      RAK.SendData("15", s.c_str());
    }
    else
    {
      fetchRetryCount++;
      delay(1000);
    }
  

  // WaterlevelProcess();
  
#endif

#ifdef SOIL
  soil.init(&Serial, &SoilSerial, MODEPIN, RS485_RI, RS485_RO);
  SoilProcess();
#endif

  bootCount++;
  RAK.sleep();
  Serial.println("LoRaWAN Modem Power Off & ESP deep sleep start. ");

  esp_deep_sleep_start();
  }
  



void loop()
{
  Serial.println("loop");
  delay(10000);
}

String bytetoStr(byte a)
{
  String ss;
  if ((a < 16))
  {
    ss = "0" + String(a, 16);
  }
  else
  {
    ss = String(a, 16);
  }

  return ss;
}

// #include <Arduino.h>
// #include <string>
// #include <Preferences.h>
// #include <stdio.h>
// #include <RAK4270/RAK4270.h>
// #include "lut-reader/lut-reader.h"
// #include <iomanip>
// #include "util/pin_configuration.h"
// #include "util/sys_config.h"
// #include <sstream> //stringstream
// #include <string>
// #include <iostream>

// using namespace std;

// #define DERE 21
// #define SURFACE_TO_PROBE 0.809
// // #define RI 18
// // #define RO 19

// // #define DEBUG

// #define WATERLEVEL
// #define SOIL

// char *DevEui = {
//     0,
// };
// char *AppEui = {
//     0,
// };

// String bytetoStr(byte a);
// // hardwareserial count from 0:
// // 0 is reserved for typical TX RX
// // 1 is for send at command

// // hardwareserial 1, 16--TX1, 17 --RX1  worked
// // hardwareserial 1, 26--TX1, 27 --RX1  worked
// // hardwareserial 2, 26--TX1, 27 --RX1 worked
// Preferences prfs;
// HardwareSerial RakSerial(1);
// RAK4270 RAK;
// LUT lut;

// int wlevelRetryCount = 0;

// void setup()
// {
//   RAK.init(&RakSerial, 16, 17);
//   lut.init();

//   // rx, tx
//   Serial.begin(115200);
//   Serial.println("Booting");

//    while (wlevelRetryCount < 10)
//   {

//     byte recv[9];
//     float waterlevel = lut.getLevel(recv); // returns waterlevel with m unit.
//     float corrected_level = (SURFACE_TO_PROBE - waterlevel);

//     // 수위센서로부터 정상적인 값을 받아오는 경우,
//     if ((recv[0] == 1) && (recv[1] == 3) && (recv[2] == 4))
//     {
//       // float to hex
//       byte arres[4];
//       String s = "00000000";

//       Serial.println();
//       for (int i = 3; i > -1; i--)
//       {
//         arres[i] = *((char *)&corrected_level + i);
//         s += bytetoStr(arres[i]);
//       }

//       String consoleStr = String(waterlevel)+ " | " +String(corrected_level);
//       Serial.println(consoleStr);
//       //RAK4270을 통해 데이터 송신

//     }
//     else
//     { //10회 실패 시,
//       wlevelRetryCount++;
//       delay(1000);
//     }

// #ifdef DEBUG
//     Serial.print("\nrecv:");
//     for (int i = 0; i < 9; i++)
//     {
//       Serial.print(recv[i], HEX);

//       Serial.print(",");
//     }
//     String ss = "00000000" + bytetoStr(recv[3]) + bytetoStr(recv[4]) + bytetoStr(recv[5]) + bytetoStr(recv[6]);
//     unsigned long hex_value = arres[3] << 24 | arres[2] << 16 | arres[1] << 8 | arres[0] float modified = *((float *)&hex_value);
//     Serial.print("expected correctedvalue : ");
//     Serial.println(corrected_level);
//     Serial.print("expected correctedvalue : ");
//     Serial.println(corrected_level);
//     Serial.print("typical : ");
//     Serial.println(ss);
//     Serial.print("modified : ");
//     Serial.println(s);
//     Serial.println(modified, 4);
// #endif
//   }

//   // RAK.setJoin();
//   // delay(2000);
//   // RAK.getStatus();
//   // delay(1000);

//   // RAK.SendData("15", "00f300f300f303e803e803e801d201d201d202b402b402b4");
//   // delay(3000);

//   // byte request[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x0c}; // inquiry for soil sensor
//   //                                                                    // uint8_t buf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x0c}; // inquiry for soil sensor

//   // // RS485 송신모드로 변경
//   // digitalWrite(modepin, MODBUS_WRITE);
//   // //데이터 전송
//   // rs485.write(request, 8);
//   // //다시 수신모드로 변경(슬레이브의 응답을 기다리는 상태)
//   // digitalWrite(modepin, MODBUS_READ);

//   // //슬레이브의 응답을 기다려야 하는 상태
//   // uint8_t response[21];
//   // if (rs485_receive(response) != -1)
//   // {
//   //   //응답이 날라온 경우

//   //   Serial.print("수신메시지:");
//   //   for (int i = 0; i < 21; i++)
//   //   {
//   //     Serial.print(response[i], HEX);
//   //     Serial.print(",");
//   //   }

//   //   // response[3] 데이터 high byte
//   //   // response[4] 데이터 low byte
//   //   Serial.println("");
//   //   temp = ((response[3] << 8) | response[4])/ 10.0;
//   //   humid = ((response[5] << 8) | response[6]) / 10.0;
//   //   ec = (response[7] << 8) | response[8];
//   //   ph = ((response[9] << 8) | response[10]) / 100.0;
//   //   N = (response[11] << 8) | response[12];
//   //   P = (response[13] << 8) | response[14];
//   //   K = (response[15] << 8) | response[16];
//   //   salt = ((response[17] << 8) | response[18])/10.0;
//   //   crc = (response[19] << 8) | response[20];
//   // }

//   //   Serial.printf("temp : %f | humid : %f | ec : %d uS | ph : %f | N : %d | P : %d | K : %d | salt : %d \n", temp, humid, ec, ph, N, P, K, salt);

//   // Set timer to 5 seconds
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//   Serial.println("\nESP gets to sleep " + String(TIME_TO_SLEEP) +
//                  " Seconds");

//   // Go to sleep now
//   esp_deep_sleep_start();
// }

// String bytetoStr(byte a)
// {
//   String ss;
//   if (a < 10)
//   {
//     ss = "0" + String(a, 16);
//   }
//   else
//   {
//     ss = String(a, 16);
//   }

//   return ss;
// }

// void loop()
// {
//  //TODO : 3 layer 센서 데이터 값 받아오고, 드라이버 설치하기

// }
