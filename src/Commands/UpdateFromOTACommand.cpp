#ifndef UPDATE_FROM_OTA_COMMAND_CLASS
#define UPDATE_FROM_OTA_COMMAND_CLASS

#include <Command.h>
#include <Logger.h>
#include <Settings.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

class UpdateFromOTACommand : public Command
{
  public:
    bool process(char *line, WiFiClient *socket) override
    {
        if (strcmp(line, "UPDATE FROM OTA") == 0)
        {
            socket->write("OK\r\n");
            delay(100);

            char *url = Settings.readOtaUrl();

            socket->write("Searching firmware updates in ");
            socket->write(url);
            socket->write("\r\n");
            socket->write("MD5 Checksum: ");
            socket->write(ESP.getSketchMD5().c_str());
            socket->write("\r\n");

            t_httpUpdate_return ret = ESPhttpUpdate.update(url);
            switch (ret)
            {
            case HTTP_UPDATE_FAILED:
                socket->write("HTTP_UPDATE_FAIL Error. ");
                socket->write(ESPhttpUpdate.getLastError());
                socket->write(": ");
                socket->write(ESPhttpUpdate.getLastErrorString().c_str());
                socket->write("\r\n");
                break;

            case HTTP_UPDATE_NO_UPDATES:
                socket->write("HTTP_UPDATE_NO_UPDATES\r\n");
                break;

            case HTTP_UPDATE_OK:
                socket->write("HTTP_UPDATE_OK\r\n");
                break;
            }

            delete url;

            return true;
        }
        return false;
    }

    void help(WiFiClient *socket) override
    {
        socket->write("UPDATE FORM OTA: check for update\r\n");
    }
};

#endif