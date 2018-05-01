#ifndef SET_BUTTON_COMMAND_CLASS
#define SET_BUTTON_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>
#include <Splitter.h>

class SetButtonNameCommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        const char *key = "SET BUTTON NAME ";
        if (memcmp(line, key, strlen(key)) == 0)
        {
            Logger.trace("Configurando nombre del botón");

            char *value = new char[strlen(line) - strlen(key) + 1];
            Splitter splitter = Splitter(line + strlen(key));

            char *index = splitter.getNextChunk(' ');
            Logger.debug(index);
            char *connectionString = splitter.getNextChunk('\0');

            if (strcmp(index, "1") == 0)
            {
                Settings.writeButtonName(1, connectionString);
                socket->write("OK\r\n");
            }
            else if (strcmp(index, "2") == 0)
            {
                Settings.writeButtonName(2, connectionString);
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
        socket->write("SET BUTTON NAME n name\r\n");
    }
};

#endif