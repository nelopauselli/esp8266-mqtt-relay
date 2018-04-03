#include <TelnetServer.h>
#include <Logger.h>

#include "Commands/DebugCommand.cpp"
#include "Commands/PingPongCommand.cpp"
#include "Commands/NotFoundCommand.cpp"

TelnetServer::TelnetServer(int port)
{
    server = new WiFiServer(port);
    server->begin();
    server->setNoDelay(true);

    add(new PingPongCommand());
    add(new DebugCommand());
    invalidCommand = new NotFoundCommand();

    Logger.trace("Ready! Use 'telnet ");
}

void TelnetServer::add(Command *command)
{
    commands[commandIndex++] = command;
}

void TelnetServer::process()
{
    //check if there are any new clients
    if (server->hasClient())
    {
        //find free/disconnected spot
        if (!socket || !socket.connected())
        {
            if (socket)
                socket.stop();
            socket = server->available();
            socket.write("Welcome to ESP!\r\n");
            Logger.trace("New client!");
            
            // Clean input stream
            while(socket.available()){
                socket.read();
            }
        }
        else
        {
            //no free/disconnected spot so reject
            WiFiClient other = server->available();
            other.write("Connection rejected");
            other.stop();
            Logger.trace("Connection rejected");
        }
    }

    //check clients for data
    if (socket && socket.connected())
    {
        if (socket.available())
        {
            //get data from the telnet client and push it to the UART
            int bufferIndex = 0;
            char buffer[50];

            while (socket.available())
            {
                char c = socket.read();
                if (c == '\r')
                {
                    // no hacemos nada con este caracter
                }
                else if (c == '\n')
                {
                    // Fin de línea para la instrucción
                    break;
                }
                else
                {
                    // vamos acumulando los caracteres de la instrucción
                    buffer[bufferIndex++] = c;
                }
            }

            buffer[bufferIndex] = '\0';
            Logger.debug(buffer);

            if (strlen(buffer) > 0)
            {
                bool found = false;
                for (int index = 0; index < commandIndex; index++)
                {
                    Command *command = commands[index];
                    if (command != NULL)
                    {
                        if (command->process(buffer, &socket))
                        {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found)
                    invalidCommand->process(buffer, &socket);
            }
        }
    }
}