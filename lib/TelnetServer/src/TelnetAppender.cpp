#ifndef TELNET_APPENDER_CLASS
#define TELNET_APPENDER_CLASS

#include <ESP8266WiFi.h>

#include "Appenders/Appender.h"

class TelnetAppender : public Appender
{
  public:
    TelnetAppender(WiFiClient *socket)
    {
        _socket = socket;
    }

    void write(const char *level, const char *message) override
    {
        if (_socket != NULL && _socket->connected())
        {
            _socket->write("[");
            _socket->write(level);
            _socket->write("] ");

            _socket->write(message);
            _socket->write("\r\n");

            _socket->flush();
        }
    }

  private:
    WiFiClient *_socket;
};

#endif