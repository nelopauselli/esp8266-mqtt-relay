#include <Arduino.h>

#define OTA_ENABLED
#define OTA_ON_LOOP

#define TEST_MODE

extern "C" {
#include "user_interface.h"
}

#include "MqttAdapter.h"
#include "MqttEventArgs.h"

#ifdef TEST_MODE
#include "ArduinoUnitTest.h"
#endif

#include "Logger.h"
#include "Appenders/SerialAppender.cpp"
#include "TraceMemory.cpp"
#include "NTPClient.h"
#include "Splitter.h"
#include "WifiAdapter.h"
#include "TelnetServer.h"
#include "Settings.h"
#include "Commands/SetWifiCommand.cpp"
#include "Commands/SetMqttCommand.cpp"
#include "Commands/SetMqttTopicBaseCommand.cpp"
#include "Commands/SetDeviceNameCommand.cpp"
#include "Relay.cpp"
#include "Observers/MqttRelayObserver.cpp"
#include "Observers/ButtonRelayObserver.cpp"
#include "Observers/ButtonMqttObserver.cpp"
#include "Button.cpp"
#include "Light.cpp"

#ifdef ARDUINO_ESP8266_NODEMCU
#define RELAY1 D5 //LED_BUILTIN
#define RELAY2 D6
#define BUTTON1 D3
#define BUTTON2 D2
#define LED_ACTIVITY D4
#define DHT_PIN D1
#define LIGHT_PIN LED_BUILTIN
#elif ARDUINO_ESP8266_ESP01
#define RELAY1 1
#define RELAY2 2
#define BUTTON1 0
#define BUTTON2 3
#elif ESP8266_D1_MINI
#define RELAY1 D5
#define RELAY2 D6
#define BUTTON1 D3
#define BUTTON2 D2
#define LED_ACTIVITY D4
#define DHT_PIN D1
#define LIGHT_PIN D7
#endif

#ifdef DHT_PIN
#include <DHT.h>
DHT dht;
#endif

#ifdef OTA_ENABLED
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#endif

Relay *relay1;
Relay *relay2;
Button *button1;
Button *button2;
#ifdef LIGHT_PIN
Light *light;
#endif

TelnetServer *telnetServer = NULL;
MqttAdapter *mqtt = NULL;

#ifdef TEST_MODE

void test1()
{
    Time time(1524362301); //22 de April de 2018 1:58:21

    char *buffer = time.toCharArray();
    Assert.areEqual("01:58:21", buffer);
}

void test2()
{
    Time time(1524434780); //22 de April de 2018 22:06:20

    char *buffer = time.toCharArray();
    Assert.areEqual("22:06:20", buffer);
}

void test3()
{
    Time time(1524389400); //22 de April de 2018 9:30:00

    char *buffer = time.toCharArray();
    Assert.areEqual("09:30:00", buffer);
}

void test4()
{
    Time time(1524393000); //22 de April de 2018 10:30:00

    char *buffer = time.toCharArray();
    Assert.areEqual("10:30:00", buffer);
}

void tests()
{
    Serial.begin(115200);
    Serial.println("Start unit tests");

    test1();
    test2();
    test3();
    test4();

    Serial.println("End unit tests");

    Assert.summary();

    delay(1000);
}

#endif

void initLogger()
{
    Logger.cleanDebug();
    Logger.cleanLog();
    Logger.debugging(false);

    Logger.add(new SerialAppender());
}

void initHardware()
{
#ifdef LED_ACTIVITY
    pinMode(LED_ACTIVITY, OUTPUT);
    digitalWrite(LED_ACTIVITY, HIGH);
#endif

    Logger.trace("Init relays...");

    relay1 = new Relay(RELAY1, "extractor", TimeSpan{1, 0, 0});
    relay2 = new Relay(RELAY2, "toallero", TimeSpan{2, 0, 0});

    Logger.trace("Init buttons...");

    button1 = new Button(BUTTON1, "boton-azul");
    button1->attach(new ButtonRelayObserver(relay1));
    button2 = new Button(BUTTON2, "boton-rojo");
    button2->attach(new ButtonRelayObserver(relay2));

#ifdef DHT_PIN
    dht.setup(DHT_PIN);
#endif

#ifdef LIGHT_PIN
    light = new Light(LIGHT_PIN, "luz");
#endif
}

void initClock()
{
    NTPClient ntpClient;
    if (!ntpClient.initClockFromServer())
        Logger.error("NTP Client init fail. :(");
}

/**
 * 
 * MQTT
 * 
 **/
void device_search()
{
    char *topicBase = Settings.readMqttTopicBase();

    char definition[512];
    strcpy(definition, "{\"topics\":[");

    if (strlen(topicBase) > 0)
    {
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay1/status\",");
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay1/on\",");
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay1/off\",");
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay2/status\",");
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay2/on\",");
        strcat(definition, "\"");
        strcat(definition, topicBase);
        strcat(definition, "/relay2/off\"");
    }
    strcat(definition, "]}");

    mqtt->publish("/devices/found", definition);

    delete topicBase;
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    char *message = new char[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    Serial.println(message);

    char *subtopic = new char[strlen(topic) - strlen(mqtt->roottopic()) + 1];
    strcpy(subtopic, topic + strlen(mqtt->roottopic()));
    Serial.print("subtopic: ");
    Serial.println(subtopic);

    MqttEventArgs args;
    args.topic = subtopic;
    args.payload = message;
    mqtt->notify(args);

    if (strcmp(topic, "/devices/search") == 0)
    {
        device_search();
    }

#ifdef LIGHT_PIN
    else if (strcmp(topic + strlen(topic) - strlen("/light"), "/light") == 0)
    {
        light->invoke(message);
    }
#endif
    else if (strcmp(topic + strlen(topic) - strlen("/restart"), "/restart") == 0)
    {
        const char *chipId = String(ESP.getChipId()).c_str();
        if (strcmp(message, chipId) == 0)
        {
            ESP.restart();
        }
        else
        {
            mqtt->publish("error", "invalid chip ID");
        }
    }
    delete message;
    delete subtopic;
}

bool reconnect()
{
    char *connectionString = Settings.readMqttConnectionString();

    Splitter splitter = Splitter(connectionString);
    char *chunk = splitter.getNextChunk('@');

    Splitter splitter2 = Splitter(chunk);
    char *mqttUserName = splitter2.getNextChunk(':');
    char *mqttPassword = splitter2.getNextChunk('\0');
    if (strlen(mqttUserName) > 0 && strlen(mqttPassword) > 0)
    {
        Logger.debug("connecting to mqtt with user & pass");
        if (mqtt->connect(mqttUserName, mqttPassword))
        {
            mqtt->setCallback(callback);
            return true;
        }
    }
    else
    {
        Logger.debug("connecting to mqtt anonymous");

        if (mqtt->connect())
        {
            mqtt->setCallback(callback);
            return true;
        }
    }
    return false;
}

bool initMQTT()
{
    char *connectionString = Settings.readMqttConnectionString();

    Splitter splitter = Splitter(connectionString);

    splitter.skipTo('@');
    char *mqttServer = splitter.getNextChunk(':');
    uint16_t mqttPort = atoi(splitter.getNextChunk('\0'));

    if (strlen(mqttServer) > 0 && mqttPort)
    {
        Logger.debug(String("Connecting to broker in ") + mqttServer + ":" + String(mqttPort));
        char *topic = Settings.readMqttTopicBase();
        char *deviceName = Settings.readDeviceName();
        mqtt = new MqttAdapter(mqttServer, mqttPort, topic, deviceName);

        relay1->attach(mqtt);
        mqtt->attach(new MqttRelayObserver(relay1));
        button1->attach(new ButtonMqttObserver(button1, mqtt));
        relay2->attach(mqtt);
        mqtt->attach(new MqttRelayObserver(relay2));
        button2->attach(new ButtonMqttObserver(button2, mqtt));
#ifdef LIGHT_PIN
        light->attach(mqtt);
#endif

        return reconnect();
    }
    return false;
}

#ifdef OTA_ENABLED
void checkForUpdates()
{
    char *url = Settings.readOtaUrl();

    Logger.trace(String("Searching firmware updates in ") + url);
    Logger.debug(String("MD5 Checksum: ") + ESP.getSketchMD5());

    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Logger.error(String("HTTP_UPDATE_FAIL Error. ") + String(ESPhttpUpdate.getLastError()) + ": " + ESPhttpUpdate.getLastErrorString());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Logger.trace("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Logger.trace("HTTP_UPDATE_OK");
        break;
    }
}
#endif

void publishInDevices()
{
    mqtt->publish("/casa/devices", "hola world!");
}

void setup()
{
#ifdef TEST_MODE
    tests();
#endif

    traceFreeMemory();

    initLogger();
    initHardware();

    Logger.debug("Getting ChipID");
    Logger.trace("ChipID: " + String(ESP.getChipId()));

    WifiAdapter.addAP(Settings.readWifi(1));
    WifiAdapter.addAP(Settings.readWifi(2));

    if (WifiAdapter.connect())
    {
#ifdef OTA_ENABLED
        checkForUpdates();
#endif

        initMQTT();

        traceFreeMemory();
        traceMemoryLeak("publish 1", &publishInDevices);
        traceMemoryLeak("publish 2", &publishInDevices);
        traceFreeMemory();

        initClock();
    }
    else
    {
        randomSeed(analogRead(A0));
        int r = random(1000, 9999);
        String ssidAP = String("ESP8266-") + String(r);
        WifiAdapter.startAsAccessPoint(ssidAP.c_str());
    }

    telnetServer = new TelnetServer(23);
    telnetServer->add(new SetWifiCommand());
    telnetServer->add(new SetMqttCommand());
    telnetServer->add(new SetMqttTopicBaseCommand());
    telnetServer->add(new SetDeviceNameCommand());

    telnetServer->start();

    Logger.trace("ready");

#ifdef LED_ACTIVITY
    digitalWrite(LED_ACTIVITY, LOW);
#endif
}

void loopMQTT()
{
    if (mqtt != NULL)
    {
        if (!mqtt->connected())
        {
            if (!reconnect())
            {
                WifiAdapter.disconnect();
                if (WifiAdapter.connect())
                {
                    initMQTT();
                }
            }
        }
        mqtt->loop();
    }
}

long lastProcess = 0;
void processButtons()
{
    button1->process();
    button2->process();
}

void processRelays()
{
    relay1->process();
    relay2->process();

    Logger.debug("Check completed.");
}

#ifdef LIGHT_PIN
void processLights()
{
    light->process();
}
#endif

void processTelnet()
{
    if (telnetServer != NULL)
        telnetServer->process();
}

#ifdef DHT_PIN
#define DHT_MAX 5
float humidity;
float temperature;
int dhtIndex = 0;

int timeSinceLastRead = 0;
void processDht()
{
    // Report every 2 seconds.
    if (millis() > timeSinceLastRead + dht.getMinimumSamplingPeriod())
    {
        float h = dht.getHumidity();
        float t = dht.getTemperature();
        timeSinceLastRead = millis();

        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t))
        {
            Logger.error("Failed to read from DHT sensor!");
            return;
        }

        humidity += h;
        temperature += t;
        Logger.debug("Humidity: " + String(h) + "%\t" + "Temperature: " + String(t) + " *C ");

        if (dhtIndex < DHT_MAX)
        {
            dhtIndex++;
        }
        else
        {
            Logger.debug("publish dht");

            float humidityAvg = humidity / (dhtIndex + 1);
            float temperatureAvg = temperature / (dhtIndex + 1);

            String message = "{\"humidity\": " + String(humidityAvg) +
                             ", \"temperature\": " + String(temperatureAvg) + "}";
            mqtt->publish("dht", message.c_str());

            humidity = 0;
            temperature = 0;
            dhtIndex = 0;
        }
    }
}
#endif

long lastBlink;
bool statusBlink = true;

#ifdef OTA_ON_LOOP
int lastUpdate = 0;
#endif

void loop(void)
{
    traceFreeMemory();

    processTelnet();

    if (!WifiAdapter.isAccessPoint())
    {
#ifdef OTA_ON_LOOP
        if (lastUpdate + 10000 < millis())
        {
            checkForUpdates();
            lastUpdate = millis();
        }
#endif

        loopMQTT();

        traceMemoryLeak("processButtons", &processButtons);

#ifdef DHT_PIN
        processDht();
#endif

        if (lastProcess + 5000 < millis())
        {
            traceMemoryLeak("processRelays", &processRelays);
#ifdef LIGHT_PIN
            traceMemoryLeak("processLights", &processLights);
            lastProcess = millis();
#endif
        }
    }

#ifdef LED_ACTIVITY
    if (lastBlink + 1000 < millis())
    {
        statusBlink = !statusBlink;
        digitalWrite(LED_ACTIVITY, statusBlink);
        lastBlink = millis();
    }
#endif

    delay(100);
}
