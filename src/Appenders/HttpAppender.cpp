#ifndef HTTP_APPENDER_CLASS
#define HTTP_APPENDER_CLASS

#include "Command.h"
#include "Appenders/Appender.h"

class HttpAppender : public Appender
{
    WiFiServer *server = NULL;
    WiFiClient socket;

  public:
    HttpAppender(int port)
    {
        server = new WiFiServer(port);
        server->begin();
        server->setNoDelay(true);
    }

    void write(const char *level, const char *message) override
    {
        if (server->hasClient())
        {
            if (socket)
            {
                socket.stop();
            }
            socket = server->available();
        }

        if (socket)
        {
            socket.write("[");
            socket.write(level);
            socket.write("] ");
            socket.write(message);
            socket.write("\r\n");
            socket.flush();

            delay(10); // go message, go!
        }
    }
};

#endif