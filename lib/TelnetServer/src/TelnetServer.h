#ifndef TELNET_SERVER_CLASS
#define TELNET_SERVER_CLASS

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <Command.h>

class TelnetServer
{
public:
  TelnetServer(int port);
  void add(Command *command);
  void process();

private:
  WiFiServer *server = NULL;
  Command *commands[10];
  int commandIndex = 0;
  WiFiClient socket;
  Command *invalidCommand = NULL;
};

#endif