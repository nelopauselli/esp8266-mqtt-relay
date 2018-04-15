#ifndef RESTART_COMMAND_CLASS
#define RESTART_COMMAND_CLASS

#include <Command.h>

class RestartCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "RESTART") == 0)
        {
            socket->write("OK\r\n");
            delay(100);

            ESP.restart();

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override{
        socket->write("RESTART: restart this device\r\n");
    }
};


#endif