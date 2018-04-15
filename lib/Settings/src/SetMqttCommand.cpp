#ifndef SET_MQTT_COMMAND_CLASS
#define SET_MQTT_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>

class SetMqttCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET MQTT ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            Logger.trace("Configurando Mqtt");

            char *value = new char[strlen(line) - strlen(key) + 1];
            strcpy(value, line + strlen(key));
            Logger.debug(value);

            Settings.writeMqttConnectionString(value);
            socket->write("OK\r\n");

            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET MQTT mqtt://username:password@server:port\r\n");
    }
};

#endif