#ifndef COMMAND_CLASS
#define COMMAND_CLASS

#include "Arduino.h"
#include <ESP8266WiFi.h>

class Command
{
  public:
    virtual bool process(char *line, WiFiClient *socket);
};

#endif