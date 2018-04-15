#ifndef HELP_COMMAND_CLASS
#define HELP_COMMAND_CLASS

#include "Command.h"
#include "CommandListItem.h"

class HelpCommand : public Command
{
  public:
    HelpCommand(CommandListItem *commands)
    {
        _commands = commands;
    }

    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "HELP") == 0)
        {
            socket->write("COMMANDS:\r\n");

            CommandListItem *item = _commands;

            while (item != NULL)
            {
                Command *command = item->command;
                command->help(socket);

                item = item->next;
            }
            socket->write("\r\n");

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("HELP: show this help\r\n");
    }

  private:
    CommandListItem *_commands;
};

#endif