#include "lut-reader.h"
#include <Arduino.h>

#define ESMUS10A

// #define LUT18

// RS485 to TTL
#define modepin 21 //통신의 송신과 수신을 변환하는 핀
#define MODBUS_READ LOW
#define MODBUS_WRITE HIGH

    SoftwareSerial rs485(19, 18);

void LUT::init()
{
    Serial.begin(115200);
    // rs485.begin(9600);                  //아두이노-rs485간 TTL통신
    pinMode(modepin, OUTPUT);           //송수신제어핀
    digitalWrite(modepin, MODBUS_READ); //수신모드
    Serial.print("Waterlevel Sensor Ready");
}

void LUT::init_new(Stream *ser, int txpin, int rxpin)
{
    _lut = ser;
  static_cast<HardwareSerial *>(_lut)->begin(9600, SERIAL_8N1, txpin, rxpin);
    // Serial.begin(115200);
    // // rs485.begin(9600);                  //아두이노-rs485간 TTL통신
    pinMode(modepin, OUTPUT);           //송수신제어핀
    digitalWrite(modepin, MODBUS_READ); //수신모드
    // Serial.print("Waterlevel Sensor Ready");
}

float LUT::getLevel(byte arr[])
{

    rs485.begin(9600); //아두이노-rs485간 TTL통신

    byte req[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xd5, 0xca}; // distance for ESMUS10A

    // byte req[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b}; // level for ESMUS10A

    // byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; // inquiry for LUT-18

    digitalWrite(modepin, MODBUS_WRITE);
    rs485.write(req, 8);
    digitalWrite(modepin, MODBUS_READ);

    byte res[9];
    if (rs485_receive(res) != -1)
    {
        //응답이 날라온 경우!
        // Serial.print("\nreceived:");
        for (int i = 0; i < 9; i++)
        {
            arr[i] = res[i];
            Serial.print(res[i], HEX);
            Serial.print(",");
        }
        // responce[3] = high byte
        // responce[4] = low byte
        //  int ret = (res[3] << 8) | res[4];

#ifdef LUT18
        unsigned long hex_value = res[3] << 24 | res[4] << 16 | res[5] << 8 | res[6];

        float converted = *((float *)&hex_value);
        //  Serial.print(" | concatenated=");
        //  Serial.print(hex_value, HEX);
        Serial.print(" | converted=");
        Serial.println(converted, 4);
        rs485.end();
        return converted;

#endif

#ifdef ESMUS10A
        unsigned long hex_value = res[3] << 8 | res[4];
        // float converted = *((int *)&hex_value) / 1000.0;
        float converted = (float)(*((int *)&hex_value)/1000.0);
        //  Serial.print(" | concatenated=");
        //  Serial.print(hex_value, HEX);
        Serial.print(" | converted=");
        Serial.println(converted);
        rs485.end();
        return (float)converted;
#endif
    }
    else
    {
        //타임아웃
        Serial.print(".");
        rs485.end();
        return -0.1;
    }
}

void LUT::end(){
    // SoftwareSerial rs485(19, 18);
    rs485.end();

}

int LUT::rs485_receive(byte recv[])
{
    //슬레이브1에서 날린 데이터가 존재할때까지 무한루프
    unsigned long t = millis(); //루프진입직전의 시간
    while (1)
    {
        if (millis() - t > 2000)
        {
            //루프안에서 누른 스톱워치하고 t하고의 시간차이가 2000ms 이상이면~
            // Serial.println("오프라인");
            return -1;
            break;
        }
        if (rs485.available())
        {
            // 485모듈에서 아두이노쪽으로 수신한값이 존재한다면~
            //데이터 읽기
            rs485.readBytes(recv, 9);
            /*
            Serial.print("(마스터)수신데이터=");
            for(int i =0;i<10;i++){
                Serial.print(recv[i],HEX);
                Serial.print(",");
            }
            Serial.println();
            */
            rs485.flush();
            return 0;
            break;
        }
    }
}



float LUT::getLevel_new(byte arr[])
{
    SoftwareSerial rs485(19, 18);

  static_cast<HardwareSerial *>(_lut)->begin(9600, SERIAL_8N1, 19, 18);

    byte req[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xd5, 0xca}; // distance for ESMUS10A

    // byte req[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b}; // level for ESMUS10A

    // byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x08, 0x44, 0x0c}; // inquiry for LUT-18

    // byte req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}; // inquiry for LUT-18

    digitalWrite(modepin, MODBUS_WRITE);
    _lut->write(req, 8);
    digitalWrite(modepin, MODBUS_READ);

    byte res[9];
    if (rs485_receive_new(res) != -1)
    {
        //응답이 날라온 경우!
        // Serial.print("\nreceived:");
        for (int i = 0; i < 9; i++)
        {
            arr[i] = res[i];
            Serial.print(res[i], HEX);
            Serial.print(",");
        }
        // responce[3] = high byte
        // responce[4] = low byte
        //  int ret = (res[3] << 8) | res[4];

#ifdef LUT18
        unsigned long hex_value = res[3] << 24 | res[4] << 16 | res[5] << 8 | res[6];

        float converted = *((float *)&hex_value);
        //  Serial.print(" | concatenated=");
        //  Serial.print(hex_value, HEX);
        Serial.print(" | converted=");
        Serial.println(converted, 4);
        // _lut.end();
        return converted;

#endif

#ifdef ESMUS10A
        unsigned long hex_value = res[3] << 8 | res[4];
        float converted = *((int *)&hex_value) / 1000.0;
        //  Serial.print(" | concatenated=");
        //  Serial.print(hex_value, HEX);
        Serial.print(" | converted=");
        Serial.println(converted);

  static_cast<HardwareSerial *>(_lut)->end();
        return (float)converted;
#endif
    }
    else
    {
        //타임아웃
        Serial.print(".");
  static_cast<HardwareSerial *>(_lut)->end();
        return -0.1;
    }
}

int LUT::rs485_receive_new(byte recv[])
{
    //슬레이브1에서 날린 데이터가 존재할때까지 무한루프
    unsigned long t = millis(); //루프진입직전의 시간
    while (1)
    {
        if (millis() - t > 2000)
        {
            //루프안에서 누른 스톱워치하고 t하고의 시간차이가 2000ms 이상이면~
            // Serial.println("오프라인");
            return -1;
            break;
        }
        if (_lut->available())
        {
            // 485모듈에서 아두이노쪽으로 수신한값이 존재한다면~
            //데이터 읽기
            _lut->readBytes(recv, 9);
            /*
            Serial.print("(마스터)수신데이터=");
            for(int i =0;i<10;i++){
                Serial.print(recv[i],HEX);
                Serial.print(",");
            }
            Serial.println();
            */
            _lut->flush();
            return 0;
            break;
        }
    }
}
