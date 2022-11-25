#include <SoftwareSerial.h>

//RS485 to TTL
// #define modepin 21 //통신의 송신과 수신을 변환하는 핀
#define MODBUS_READ LOW
#define MODBUS_WRITE HIGH

class LUT{
    public:
        void init();
        void init_new(Stream *ser, int txpin, int rxpin);
        float getLevel(byte arr[]);
        int rs485_receive(byte recv[]);
        float getLevel_new(byte arr[]);
        int rs485_receive_new(byte recv[]);
        void end();

    private:
        Stream* _lut;

};