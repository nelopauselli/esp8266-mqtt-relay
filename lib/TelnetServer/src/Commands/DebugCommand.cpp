#ifndef DEBUG_COMMAND_CLASS
#define DEBUG_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <TelnetAppender.cpp>

class DebugCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "DEBUG") == 0)
        {
            Logger.add(new TelnetAppender(socket));
            socket->write("OK\r\n");
            delay(1);

            return true;
        }
        return false;
    }
};

#endif