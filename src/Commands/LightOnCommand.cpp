#ifndef LIGHT_ON_COMMAND_CLASS
#define LIGHT_ON_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>

class LightOnCommand : public Command
{
  public:
    LightOnCommand(int pin)
    {
        _pin = pin;
    }
    
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "TURN LIGHT ON") == 0)
        {
            Logger.trace("Encendiendo luz");
            digitalWrite(_pin, LOW);
            socket->write("OK\r\n");

            delay(1);

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("SET LIGHT ON: Enciende la luz\r\n");
    }

  private:
    int _pin = 0;
};

#endif