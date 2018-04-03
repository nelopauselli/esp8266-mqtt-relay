#ifndef PING_COMMAND_CLASS
#define PING_COMMAND_CLASS

#include <Command.h>

class PingPongCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "PING") == 0)
        {
            socket->write("PONG\r\n");
            delay(1);

            return true;
        }
        return false;
    }
};


#endif