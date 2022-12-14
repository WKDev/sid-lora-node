#include <Arduino.h>
#include "Stream.h"

class RAK4270
{
public:
    char *DevEui;
    char *AppEui;

    int _TimeOut = 5000;

  void init(Stream *ser, int txpin, int rxpin);
    void getStatus();
    void SendData(const char *port, const char *payload);
    char *getDevEui();
    char *getAppEui();
    void setAppEui(String phrase);
    void setJoin();

    void restart();
    void sleep();
    void wakeup();

private:
    Stream* _rak;

};

