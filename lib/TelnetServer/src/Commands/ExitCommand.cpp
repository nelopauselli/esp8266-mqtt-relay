#ifndef EXIT_COMMAND_CLASS
#define EXIT_COMMAND_CLASS

#include <Command.h>

class ExitCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "EXIT") == 0)
        {
            socket->write("BYE BYE\r\n");
            delay(100);

            socket->stop();

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override{
        socket->write("EXIT: close telnet socket\r\n");
    }
};


#endif