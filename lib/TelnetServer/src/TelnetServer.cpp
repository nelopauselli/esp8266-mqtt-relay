#include <TelnetServer.h>

#include "Logger.h"
#include "Commands/PingPongCommand.cpp"
#include "Commands/ExitCommand.cpp"
#include "Commands/RestartCommand.cpp"
#include "Commands/ResetCommand.cpp"
#include "Commands/NotFoundCommand.cpp"
#include "Commands/HelpCommand.cpp"

TelnetServer::TelnetServer(int port)
{
    _port = port;
    // add a PingPong command handler
    add(new PingPongCommand());
}

void TelnetServer::add(Command *command)
{
    // Add the new command at end of list
    if (commands == NULL)
        commands = new CommandListItem(command);
    else
    {
        CommandListItem *item = commands;
        while (item->next != NULL)
        {
            item = item->next;
        }
        item->next = new CommandListItem(command);
    }
}

void TelnetServer::start()
{
    // complete the commands list with help and NotFound
    add(new ExitCommand());
    add(new RestartCommand());
    add(new ResetCommand());
    add(new HelpCommand(commands));
    add(new NotFoundCommand());

    server = new WiFiServer(_port);
    server->begin();
    server->setNoDelay(true);

    _stated = true;
    
    Logger.trace("Telnet is ready!");
}

void TelnetServer::process()
{
    if (!_stated)
    {
        Logger.error("Telnet should be started before process!");
        return;
    }

    // check if there are any new clients
    if (server->hasClient())
    {
        // only one client !
        if (!socket || !socket.connected())
        {
            if (socket)
                socket.stop();
            socket = server->available();

            socket.write("\r\n");
            socket.write(" ####### ####### #       #     # ####### #######                     \r\n");
            socket.write("    #    #       #       ##    # #          #           ####  #    # \r\n");
            socket.write("    #    #       #       # #   # #          #          #    # ##   # \r\n");
            socket.write("    #    #####   #       #  #  # #####      #          #    # # #  # \r\n");
            socket.write("    #    #       #       #   # # #          #          #    # #  # # \r\n");
            socket.write("    #    #       #       #    ## #          #          #    # #   ## \r\n");
            socket.write("    #    ####### ####### #     # #######    #           ####  #    # \r\n");
            socket.write("\r\n");
            socket.write("    #######  #####  ######   #####   #####   #####   #####  \r\n");
            socket.write("    #       #     # #     # #     # #     # #     # #     # \r\n");
            socket.write("    #       #       #     # #     #       # #       #       \r\n");
            socket.write("    #####    #####  ######   #####   #####  ######  ######  \r\n");
            socket.write("    #             # #       #     # #       #     # #     # \r\n");
            socket.write("    #       #     # #       #     # #       #     # #     # \r\n");
            socket.write("    #######  #####  #        #####  #######  #####   #####  \r\n");
            socket.write("\r\n");
            socket.write("Type HELP to list commands.\r\n");
            socket.write("$ ");

            // Clean input stream
            while (socket.available())
            {
                socket.read();
            }
        }
        else
        {
            // other client is working already, then incoming client is rejected
            WiFiClient other = server->available();
            other.write("Connection rejected");
            other.stop();
        }
    }

    // check client for data
    if (socket && socket.connected())
    {
        if (socket.available())
        {
            //get data from the client until '\n' (and skip '\r')
            int bufferIndex = 0;
            char buffer[50];

            while (socket.available())
            {
                char c = socket.read();
                if (c == '\r')
                {
                    // nothing to do
                }
                else if (c == '\n')
                {
                    // end of command
                    break;
                }
                else
                {
                    // adding chars to build command
                    buffer[bufferIndex++] = c;
                }
            }

            // ending command with a '\0'
            buffer[bufferIndex] = '\0';
            Logger.debug(buffer);

            if (strlen(buffer) > 0) // skip blank command
            {
                CommandListItem *item = commands;
                while (item != NULL)
                {
                    // iterating commands list waiting a handler be able to handle the command
                    Command *command = item->command;
                    if (command != NULL)
                    {
                        if (command->process(buffer, &socket))
                        {
                            // stop iteration when a command can handle the command
                            break;
                        }
                    }
                    item = item->next;
                }

                // tell to user that server is ready to another command
                socket.write("$ ");
            }
        }
    }
}