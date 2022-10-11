#include "soil-reader.h"
#include <Arduino.h>

//RS485 to TTL
#define modepin 21 //통신의 송신과 수신을 변환하는 핀
#define MODBUS_READ LOW
#define MODBUS_WRITE HIGH

SoftwareSerial rs485_soil(19, 18);

void Soil::init()
{
    Serial.begin(115200);
    rs485_soil.begin(9600);                  //아두이노-rs485간 TTL통신
    pinMode(modepin, OUTPUT);           //송수신제어핀
    digitalWrite(modepin, MODBUS_READ); //수신모드
    Serial.print("Waterlevel Sensor Ready");
}

float Soil::getData(byte arr[])
{
    // byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; // inquiry for LUT-18
    byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x0c}; // inquiry for LUT-18

    digitalWrite(modepin, MODBUS_WRITE);
    rs485_soil.write(req, 8);
    digitalWrite(modepin, MODBUS_READ);

    byte res[50];
    if (rs485_receive(res) != -1)
    {
        //응답이 날라온 경우!
        Serial.print("\nreceived:");
        for (int i = 0; i < 21; i++)
        {
            arr[i] = res[i];
            Serial.print(res[i], HEX);
            Serial.print(",");
        }
        // responce[3] = high byte
        // responce[4] = low byte
        //  int ret = (res[3] << 8) | res[4];
        unsigned long hex_value = res[3] << 24 | res[4] << 16 | res[5] << 8 | res[6];
        
        float converted = *((float *)&hex_value);
        //  Serial.print(" | concatenated=");
        //  Serial.print(hex_value, HEX);
        Serial.print(" | converted=");
        Serial.println(converted, 4);

        return converted;    
    }
    else
    {
        //타임아웃
        Serial.print(".");
        return -0.1;
    }
}

    
    int Soil::rs485_receive(byte recv[]){
    //슬레이브1에서 날린 데이터가 존재할때까지 무한루프
    unsigned long t = millis(); //루프진입직전의 시간
    while(1){
        if(millis() - t > 2000){
        //루프안에서 누른 스톱워치하고 t하고의 시간차이가 2000ms 이상이면~
        //Serial.println("오프라인");
        return -1;
        break;
        }
        if(rs485_soil.available()){
        //485모듈에서 아두이노쪽으로 수신한값이 존재한다면~
        //데이터 읽기
        rs485_soil.readBytes(recv,21);
        /*
        Serial.print("(마스터)수신데이터=");
        for(int i =0;i<10;i++){
            Serial.print(recv[i],HEX);
            Serial.print(",");
        }
        Serial.println();
        */
        rs485_soil.flush();
        return 0;
        break;
        }
    }
    }


