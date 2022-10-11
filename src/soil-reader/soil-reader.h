#include <SoftwareSerial.h> // SoftwareSerial for RS485


class Soil{
    public:
        void init();
        float getData(byte arr[]);
        int rs485_receive(byte recv[]);

};