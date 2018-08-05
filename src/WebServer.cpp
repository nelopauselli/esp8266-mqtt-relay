#ifndef WEB_SERVER_CLASS
#define WEB_SERVER_CLASS

#include <ESP8266WebServer.h>
#include <Logger.h>
#include <Settings.h>

class WebServer
{
  public:
    WebServer()
    {
        _server = new ESP8266WebServer(80);
    }

    ~WebServer()
    {
        delete _server;
    }

    void setup()
    {
        _server->on("/", HTTP_OPTIONS, [&]() {
            _server->sendHeader("Access-Control-Max-Age", "10000");
            _server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
            _server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
            _server->sendHeader("Access-Control-Allow-Origin", "*");
            _server->send(200, "text/plain", "");
        });

        _server->on("/", HTTP_GET, [&]() {
            DEBUGLN("Incomming GET");
            _server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
            _server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
            _server->sendHeader("Access-Control-Allow-Origin", "*");
            _server->send(200, "text/plain", "I'm a IoT Device");
        });

        _server->on("/", HTTP_POST, [&]() {
            DEBUGLN("Incomming POST");

            char buffer[64];
            if (_server->hasArg("ssid") && _server->hasArg("password"))
            {
                strcpy(buffer, _server->arg("ssid").c_str());
                strcat(buffer, "@");
                strcat(buffer, _server->arg("password").c_str());
                Settings.writeWifi(1, buffer);
            }
            if (_server->hasArg("mqttConnectionString"))
            {
                strcpy(buffer, _server->arg("mqttConnectionString").c_str());
                Settings.writeMqttConnectionString(buffer);
            }
            if (_server->hasArg("mqttTopicBase"))
            {
                strcpy(buffer, _server->arg("mqttTopicBase").c_str());
                Settings.writeMqttTopicBase(buffer);
            }
            if (_server->hasArg("deviceName"))
            {
                strcpy(buffer, _server->arg("deviceName").c_str());
                Settings.writeDeviceName(buffer);
            }

            _server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
            _server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
            _server->sendHeader("Access-Control-Allow-Origin", "*");
            _server->send(201, "text/plain", "See you later!");
        });

        _server->onNotFound([&]() {
            _server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
            _server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
            _server->sendHeader("Access-Control-Allow-Origin", "*");
            _server->send(404, "text/plain", "404: Not found");
        });

        _server->begin(); // Actually start the server
        DEBUGLN("HTTP server started");
    }

    void loop(void)
    {
        _server->handleClient(); // Listen for HTTP requests from clients
    }

  private:
    ESP8266WebServer *_server;
};

#endif
