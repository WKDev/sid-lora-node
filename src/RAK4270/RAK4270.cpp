#include "RAK4270.h"

void RAK4270::init(Stream *ser, int txpin, int rxpin)
{

  _rak = ser;
  static_cast<HardwareSerial *>(_rak)->begin(115200, SERIAL_8N1, txpin, rxpin);

  // _rak->begin(115200, SERIAL_8N1, txpin, rxpin);
}

void RAK4270::getStatus()
{
  _rak->write("at+get_config=lora:status\r\n");
  delay(100);

  if (_rak->available())
  {
    String resp = _rak->readString();
    Serial.println(resp);
  }
}

void RAK4270::setJoin()
{
  char recvString[32] = {
      0,
  };
  int retryCount = 0;

  while (true)
  {
    _rak->write("at+join\r\n");
    delay(2000);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    Serial.print("cmd : [at+join] | resp : ");
    for (int j = 0; j < 29; j++)
    {
      if (recvString[j] != '\0')
      {
        Serial.print(recvString[j]);
      }
    }
    // Serial.println("");

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      break;
    }
    else
    {
      break;

      // retryCount++;
      // if (retryCount < 5)
      // {
      //   Serial.print("..");
      // }
      // else
      // {
      //   Serial.println("AT Join Failed 5 times! _rak & ESP reset.");

      //   // _rak->write("at+set_config=device:restart\r\n");
      //   delay(100);

      //   // ESP.restart();
    }
  }
  Serial.println("");

  delay(_TimeOut);
}

void RAK4270::SendData(const char *port, const char *payload)
{

  char recvString[60] = {
      0,
  };
  int retryCount = 0;

  String cmd = "at+send=lora:" + String(port) + ":" + payload;

  while (true)
  {
    _rak->write("at+send=lora:");
    _rak->write(port);
    _rak->write(":");
    _rak->write(payload);
    _rak->write("\r\n");

    delay(500);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    Serial.print("cmd : " + cmd + " | resp : ");

    for (int j = 0; j < 60; j++)
    {
      if (recvString[j] != '0' && recvString[j] != 0 && recvString[j] != '\0')
      {
        Serial.print(recvString[j]);
      }
    }

    if (recvString[0] == 'E' && recvString[1] == 'R')
    {

      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT Send Failed 5 times! _rak & ESP reset.");

        // _rak->write("at+set_config=device:restart\r\n");
        delay(100);

        // ESP.restart();
        break;
      }
    }
    else if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      Serial.println("AT Send Success.");
      break;
    }

    Serial.println("");

    delay(_TimeOut);
  }
}

char *RAK4270::getDevEui()
{
  char recvString[1000] = {
      'a',
  };

  //  char *eui = (char *)malloc(sizeof(char) * 16);
  char *eui = new char[16];
  eui[0] = '1';

  _rak->write("at+get_config=lora:status\r\n");
  if (_rak->available())
  {
    String resp = _rak->readString();
    Serial.println(resp);
  }

  // int retryCount = 0;
  // char byteRead;

  // while (true)
  // {
  //   _rak->write("at+get_config=lora:status\r\n");

  //   delay(500);

  //   int availableBytes = _rak->available();

  //   int euiCounter = 0;
  //   bool enEuiCount = false;
  //   for (int i = 0; i < availableBytes; i++)
  //   {
  //     // recvString[i] = _rak->read();

  //     Serial.print(recvString[i]);

  //     if (i > 10)
  //     {
  //       if (recvString[i - 8] == 'D' && recvString[i - 7] == 'e' && recvString[i - 6] == 'v' && recvString[i - 5] == 'E' && recvString[i - 4] == 'u' && recvString[i - 3] == 'i')
  //       {
  //         enEuiCount = true;
  //         // Serial.println("euEuicount true");
  //       }
  //       if (enEuiCount)
  //       {
  //         // Serial.print("euiCounter : ");
  //         // Serial.println(euiCounter);
  //         eui[euiCounter] = recvString[i];
  //         // Serial.print(resDevEui[euiCounter]);

  //         euiCounter++;

  //         if (euiCounter > 15)
  //         {
  //           enEuiCount = false;
  //           // Serial.println("euEuicount false");
  //         }
  //       }
  //     }
  //   }
  //   _rak->flush();

  //   Serial.print("cmd : at+get_config=lora:status | resp : ");

  //   // for (int j = 0; j < 16; j++)
  //   // {
  //   //   Serial.print(eui[j]);
  //   // }
  //   // Serial.println("");

  //   if (recvString[0] == 'O' && recvString[1] == 'K')
  //   {
  //     Serial.println("AT status Success.");
  //     break;
  //   }
  //   else
  //   {
  //     retryCount++;
  //     if (retryCount < 5)
  //     {
  //       Serial.print("..");
  //     }
  //     else
  //     {
  //       Serial.println("AT status Failed 5 times! _rak & ESP reset.");

  //       _rak->write("at+set_config=device:restart\r\n");
  //       delay(100);

  //       ESP.restart();
  //     }
  //   }
  //   Serial.println("");

  delay(_TimeOut);
  // }

  return eui;
}

char *RAK4270::getAppEui()
{
  char recvString[1000] = {
      0,
  };

  //  char *eui = (char *)malloc(sizeof(char) * 16);
  char *eui = new char[16];

  int retryCount = 0;

  while (true)
  {
    _rak->write("at+get_config=lora:status\r\n");

    delay(500);

    int availableBytes = _rak->available();

    int euiCounter = 0;
    bool enEuiCount = false;
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();

      Serial.print(recvString[i]);

      if (i > 10)
      {
        if (recvString[i - 8] == 'A' && recvString[i - 7] == 'p' && recvString[i - 6] == 'p' && recvString[i - 5] == 'E' && recvString[i - 4] == 'u' && recvString[i - 3] == 'i')
        {
          Serial.print("AppEui found at position ");
          Serial.println(i);
          enEuiCount = true;
          // Serial.println("euEuicount true");
        }
        if (enEuiCount)
        {
          // Serial.print("euiCounter : ");
          // Serial.println(euiCounter);
          eui[euiCounter] = recvString[i];
          // Serial.print(resDevEui[euiCounter]);

          euiCounter++;

          if (euiCounter > 15)
          {
            enEuiCount = false;
            // Serial.println("euEuicount false");
          }
        }
      }
    }
    _rak->flush();

    Serial.print("cmd : at+get_config=lora:status | resp : ");

    // for (int j = 0; j < 16; j++)
    // {
    //   Serial.print(eui[j]);
    // }
    // Serial.println("");

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      Serial.println("AT status Success.");
      break;
    }
    else
    {
      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT status Failed 5 times! _rak & ESP reset.");

        _rak->write("at+set_config=device:restart\r\n");
        delay(100);

        ESP.restart();
      }
    }
    Serial.println("");

    delay(_TimeOut);
  }

  return eui;
}

void RAK4270::setAppEui(String phrase)
{
  char recvString[32] = {
      0,
  };
  int retryCount = 0;

  while (true)
  {

    String cmd = "at+set_config=lora:app_eui:";
    cmd += phrase.c_str();
    cmd += "\r\n";
    _rak->write(cmd.c_str());
    delay(500);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    Serial.print("cmd : [at+set_config=lora:app_eui:] | resp : ");

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      Serial.println("AT Join Success. Keep going on.");
      break;
    }
    else
    {
      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT at+set_config=lora:app_eu Failed 5 times! _rak & ESP reset.");

        _rak->write("at+set_config=device:restart\r\n");
        delay(100);

        ESP.restart();
      }
    }
    Serial.println("");

    delay(_TimeOut);
  }
}

void RAK4270::restart()
{
  char recvString[32] = {
      0,
  };
  int retryCount = 0;

  while (true)
  {
    _rak->write("at+set_config=device:restart\r\n");
    delay(500);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    Serial.print("cmd : [at+set_config=device:restart] | resp : ");
    // for (int j = 0; j < 29; j++)
    // {
    //         if (recvString[j]!='0'){
    // Serial.print(recvString[j]);
    //       Serial.print(" ");
    //}
    // }

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      Serial.println("AT Success. restart device.");
      break;
    }
    else
    {
      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT restart Failed 5 times! _rak & ESP reset.");

        _rak->write("at+set_config=device:restart\r\n");
        delay(100);

        ESP.restart();
      }
    }
    Serial.println("");

    delay(_TimeOut);
  }
}

void RAK4270::sleep()
{
  char recvString[20] = {
      0,
  };
  int retryCount = 0;

  while (true)
  {
    _rak->write("at+set_config=device:sleep:1\r\n");
    delay(500);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    Serial.print("cmd : [at+set_config=device:restart] | resp : ");
    // for (int j = 0; j < 29; j++)
    // {
    //         if (recvString[j]!='0'){
    // Serial.print(recvString[j]);
    //       Serial.print(" ");
    //}
    // }

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      String s = "";
      for (int i = 0; i < 20; i++)
      {
        s += String(recvString[i]);
      }

      Serial.println(s);
      break;
    }
    else
    {
      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT sleep Failed 5 times! _rak & ESP reset.");
        delay(100);
      }
    }
    Serial.println("");

    delay(_TimeOut);
  }
}

void RAK4270::wakeup()
{
  char recvString[20] = {
      0,
  };
  int retryCount = 0;

  while (true)
  {
    _rak->write("at+set_config=device:sleep:0\r\n");
    delay(500);

    int availableBytes = _rak->available();
    for (int i = 0; i < availableBytes; i++)
    {
      recvString[i] = _rak->read();
    }
    _rak->flush();

    if (recvString[0] == 'O' && recvString[1] == 'K')
    {
      String s = "";
      for (int i = 0; i < 20; i++)
      {
        s += String(recvString[i]);
      }

      Serial.println(s);
      break;
    }
    else
    {
      retryCount++;
      if (retryCount < 5)
      {
        Serial.print("..");
      }
      else
      {
        Serial.println("AT sleep Failed 5 times! _rak & ESP reset.");
        delay(100);
      }
    }
    Serial.println("");

    delay(_TimeOut);
  }
}
