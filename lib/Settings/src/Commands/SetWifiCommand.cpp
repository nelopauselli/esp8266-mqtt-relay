#ifndef SET_WIFI_COMMAND_CLASS
#define SET_WIFI_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>
#include "Splitter.h"

class SetWifiCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET WIFI ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            DEBUGLN("Configurando WiFi");

            Splitter splitter = Splitter(line + strlen(key));

            char *index = splitter.getNextChunk(' ');
            char *connectionString = splitter.getNextChunk('\0');

            if (strcmp(index, "1") == 0)
            {
                Settings.writeWifi(1, connectionString);
                socket->write("OK\r\n");
            }
            else if (strcmp(index, "2") == 0)
            {
                Settings.writeWifi(2, connectionString);
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