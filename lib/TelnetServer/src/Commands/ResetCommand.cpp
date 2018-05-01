#ifndef RESET_COMMAND_CLASS
#define RESET_COMMAND_CLASS

#include <Command.h>

class ResetCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "RESET") == 0)
        {
            socket->write("BYE BYE\r\n");
            delay(100);

            socket->stop();
            delay(100);

            ESP.reset();

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override{
        socket->write("RESET: reset this device\r\n");
    }
};


#endif