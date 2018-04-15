#ifndef WIFI_SETTER_COMMAND_CLASS
#define WIFI_SETTER_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>

class WifiSetterCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET WIFI ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            Logger.trace("Configurando WiFi");

            char *value = new char[strlen(line) - strlen(key) + 1];
            strcpy(value, line + strlen(key));
            Logger.debug(value);

            char *index = strtok(value, " ");
            Logger.debug(index);
            char *ssid = strtok(NULL, "@");
            Logger.debug(ssid);
            char *password = strtok(NULL, "@");
            Logger.debug(password);

            if (strcmp(index, "1") == 0)
            {
                Settings.writeSSID(1, ssid);
                Settings.writePassword(1, password);
                socket->write("OK\r\n");
            }
            else if (strcmp(index, "2") == 0)
            {
                Settings.writeSSID(2, ssid);
                Settings.writePassword(2, password);
                socket->write("OK\r\n");
            }
            else
            {
                socket->write("INVALID INDEX\r\n");
            }
            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET WIFI n ssid@password\r\n");
    }
};

#endif