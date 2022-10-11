#include <SoftwareSerial.h> // SoftwareSerial for RS485
#include <Arduino.h>
#include "Stream.h"

class SoilDriver{
    public:
        void init(Stream *debugSer, SoftwareSerial *ser, int modepin, int txpin, int rxpin);
       void getData(byte layer,byte arr[]);
        int rs485_receive(byte recv[]);
        
    private:
        SoftwareSerial* _soil;
        Stream* DebugSer;

        int _TimeOut = 5000;
        int _modepin;
};