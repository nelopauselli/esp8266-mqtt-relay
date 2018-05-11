#ifndef SET_MQTT_TOPIC_BASE_COMMAND_CLASS
#define SET_MQTT_TOPIC_BASE_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>
#include "Splitter.h"

class SetMqttTopicBaseCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET TOPIC BASE ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            DEBUG("Configurando topic base ");

            char *value = line + strlen(key);
            DEBUGLN(value);

            Settings.writeMqttTopicBase(value);
            socket->write("OK\r\n");
            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET TOPIC BASE /topic/base\r\n");
    }
};

#endif