#include <SoftwareSerial.h> // SoftwareSerial for RS485

class LUT{
    public:
        void init();
        float getLevel(byte arr[]);
        int rs485_receive(byte recv[]);


    private:
        Stream* _lut;

};