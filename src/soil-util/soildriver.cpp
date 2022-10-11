#include "soildriver.h"
#include <Arduino.h>

// RS485 to TTL
#define MODBUS_READ LOW
#define MODBUS_WRITE HIGH
#define RECV_LENGTH 21

void SoilDriver::init(Stream *debugser, SoftwareSerial *ser, int modepin, int txpin, int rxpin)
{
    DebugSer = debugser;
    _soil = ser;
    _modepin = modepin;
    _soil->begin(9600);

    pinMode(_modepin, OUTPUT);           //송수신제어핀
    digitalWrite(_modepin, MODBUS_READ); //수신모드
    DebugSer->print("soil Sensor Ready");
}

void SoilDriver::getData(byte layer, byte arr[])
{

    byte req1[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x0c}; // inquiry for Layer1
    byte req2[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x3f}; // inquiry for Layer2
    byte req3[] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x08, 0x45, 0xee}; // inquiry for Layer3
    // byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; // inquiry for LUT-18

    byte *ret = new byte[RECV_LENGTH];

    digitalWrite(_modepin, MODBUS_WRITE);

    if (layer == 1)
    {
        this->_soil->write(req1, 8);
    }
    else if (layer == 2)
    {
        this->_soil->write(req2, 8);
    }
    else if (layer == 3)
    {
        this->_soil->write(req3, 8);
    }
    
    digitalWrite(_modepin, MODBUS_READ);

    byte res[RECV_LENGTH] = {
        0,
    };
    if (rs485_receive(res) != -1)
    {
        //응답이 날라온 경우!
        // DebugSer->print("\nreceived:");
        for (int i = 0; i < RECV_LENGTH; i++)
        {
            arr[i] = res[i];
            // DebugSer->print(res[i], HEX);
            // DebugSer->print(",");
        }

        // responce[3] = high byte
        // responce[4] = low byte
        //  int ret = (res[3] << 8) | res[4];
        // unsigned long hex_value = res[3] << 24 | res[4] << 16 | res[5] << 8 | res[6];
        // float converted = *((float *)&hex_value);
        // //  Serial.print(" | concatenated=");
        // //  Serial.print(hex_value, HEX);
        // DebugSer->print(" | converted=");
        // DebugSer->println(converted, 4);
    }
    else
    {
        //타임아웃
        this->DebugSer->println("NO Response from soil.");

        for (int i = 0; i < 100; i++)
        {
            ret[i] = 0;
        }
    }
}

int SoilDriver::rs485_receive(byte recv[])
{
    //슬레이브1에서 날린 데이터가 존재할때까지 무한루프
    unsigned long t = millis(); //루프진입직전의 시간
    while (1)
    {
        if (millis() - t > 2000)
        {
            //루프안에서 누른 스톱워치하고 t하고의 시간차이가 2000ms 이상이면~
            this->DebugSer->println("오프라인");
            return -1;
            break;
        }
        if (this->_soil->available())
        {
            // 485모듈에서 아두이노쪽으로 수신한값이 존재한다면~
            //데이터 읽기
            this->_soil->readBytes(recv, RECV_LENGTH);

            // this->DebugSer->print("(마스터)수신데이터=");
            // for(int i =0;i<RECV_LENGTH;i++){
            // this->DebugSer->print(recv[i],HEX);
            // this->DebugSer->print(",");
            // }
            this->DebugSer->println();

            this->_soil->flush();
            return 0;
            break;
        }
    }
}
