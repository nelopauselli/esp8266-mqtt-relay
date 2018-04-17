#ifndef LIGHT_OFF_COMMAND_CLASS
#define LIGHT_OFF_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>

class LightOffCommand : public Command
{
  public:
    LightOffCommand(int pin)
    {
        _pin = pin;
    }
    
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "TURN LIGHT OFF") == 0)
        {
            Logger.trace("Apangando luz");
            digitalWrite(_pin, HIGH);
            socket->write("OK\r\n");

            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET LIGHT OFF: Apaga la luz\r\n");
    }

  private:
    int _pin = 0;
};

#endif