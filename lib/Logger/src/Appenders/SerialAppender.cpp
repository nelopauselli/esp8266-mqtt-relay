#ifndef SERIAL_APPENDER_CLASS
#define SERIAL_APPENDER_CLASS

#include "Arduino.h"
#include "Appender.h"

class SerialAppender : public Appender
{
  public:
    void write(const char *level, const char *message) override
    {
        Serial.write("[");
        Serial.write(level);
        Serial.write("] ");
        
        Serial.write(message);
        Serial.write("\r\n");
    }
};

#endif