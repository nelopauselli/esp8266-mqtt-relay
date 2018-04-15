#ifndef NOTFOUND_COMMAND_CLASS
#define NOTFOUND_COMMAND_CLASS

#include <Command.h>

class NotFoundCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        socket->write("Invalid command\r\n");
        delay(1);
        return true;
    }

    void help(WiFiClient *socket) override
    {
    }
};

#endif