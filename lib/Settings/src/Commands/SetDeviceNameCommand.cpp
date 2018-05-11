#ifndef SET_DEVICE_NAME_COMMAND_CLASS
#define SET_DEVICE_NAME_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>
#include "Splitter.h"

class SetDeviceNameCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET DEVICE NAME ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            DEBUG("Configurando device name ");

            char *value = line + strlen(key);
            DEBUGLN(value);

            Settings.writeDeviceName(value);
            socket->write("OK\r\n");
            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET DEVICE NAME devicename\r\n");
    }
};

#endif