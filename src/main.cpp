#include <Arduino.h>

#define HARDWARE_MONITORING

#ifndef RELEASE
#define DEBUG(...) Serial.print(__VA_ARGS__)
#define DEBUGLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif

#ifndef RELEASE
#define TEST_MODE
#define DEBUG_TO_SERIAL
//#define DEBUG_BY_HTTP
#endif

extern "C"
{
#include "user_interface.h"
}

#include "MqttAdapter.h"
char *mqttUserName = "";
char *mqttPassword = "";

#ifdef TEST_MODE
#include "ArduinoUnitTest.h"
#endif

#include "Logger.h"
#include "Appenders/SerialAppender.cpp"
#ifdef DEBUG_BY_HTTP
#include "Appenders/HttpAppender.cpp"
#endif

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
#include "Commands/SetRelayNameCommand.cpp"
#include "Commands/SetButtonNameCommand.cpp"
#include "Commands/UpdateFromOTACommand.cpp"
#include "Relay.cpp"
#include "Observers/MqttRelayObserver.cpp"
#include "Observers/RelayMqttObserver.cpp"
#include "Observers/ButtonRelayObserver.cpp"
#include "Observers/ButtonMqttObserver.cpp"
#include "Button.cpp"
#include "Light.cpp"

#ifdef HARDWARE_MONITORING
#include "HardwareMonitoring.h"
#include "Observers/HardwareMonitoringMqttObserver.cpp"
HardwareMonitoring hardwareMonitoring;
#endif

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
#define LED_ACTIVITY D7
#define DHT_PIN D1
#define LIGHT_PIN D4
#endif

#ifdef DHT_PIN
#include <DHT.h>
DHT dht;
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
    Logger.trace("Start unit tests");

    test1();
    test2();
    test3();
    test4();

    Logger.trace("End unit tests");

    Assert.summary();

    delay(1000);
}

#endif

void initLogger()
{
    Logger.cleanDebug();
    Logger.cleanLog();
    Logger.debugging(false);

#ifdef DEBUG_TO_SERIAL
    Logger.add(new SerialAppender());
#endif
}

void initHardware()
{
#ifdef LED_ACTIVITY
    pinMode(LED_ACTIVITY, OUTPUT);
    digitalWrite(LED_ACTIVITY, HIGH);
#endif

    Logger.trace("Init relays...");

    relay1 = new Relay(RELAY1, Settings.readRelayName(1), TimeSpan{1, 0, 0});
    relay2 = new Relay(RELAY2, Settings.readRelayName(2), TimeSpan{1, 0, 0});

    Logger.trace("Init buttons...");

    button1 = new Button(BUTTON1, Settings.readButtonName(1));
    button1->attach("button-1 => relay-1", new ButtonRelayObserver(relay1));
    button2 = new Button(BUTTON2, Settings.readButtonName(2));
    button2->attach("button-2 => relay-2", new ButtonRelayObserver(relay2));

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

    mqtt->publish("devices/found", definition);

    delete topicBase;
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Logger.debug("Message arrived [" + String(topic) + "] ");

    char *message = new char[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    Logger.debug(message);

    char *subtopic = new char[strlen(topic) - strlen(mqtt->roottopic()) + 1];
    strcpy(subtopic, topic + strlen(mqtt->roottopic()));

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
    Logger.debug("connecting to mqtt with user & pass");
    if (mqtt->connect(mqttUserName, mqttPassword))
    {
        mqtt->setCallback(callback);
        return true;
    }
    return false;
}

bool initMQTT()
{
    char *connectionString = Settings.readMqttConnectionString();
    Splitter splitter = Splitter(connectionString);

    char *chunk = splitter.getNextChunk('@');

    Splitter splitter2 = Splitter(chunk);
    mqttUserName = splitter2.getNextChunk(':');
    mqttPassword = splitter2.getNextChunk('\0');

    char *mqttServer = splitter.getNextChunk(':');
    uint16_t mqttPort = atoi(splitter.getNextChunk('\0'));

    if (strlen(mqttServer) > 0 && mqttPort)
    {
        Logger.debug(String("Connecting to broker in ") + mqttServer + ":" + String(mqttPort));
        char *topic = Settings.readMqttTopicBase();
        char *deviceName = Settings.readDeviceName();
        mqtt = new MqttAdapter(mqttServer, mqttPort, topic, deviceName);

        relay1->attach("relay-1 => mqtt", new RelayMqttObserver(relay1, mqtt));
        mqtt->attach("mqtt => relay-1", new MqttRelayObserver(relay1));
        button1->attach("button-1 => mqtt", new ButtonMqttObserver(button1, mqtt));
        relay2->attach("relay-2 => mqtt", new RelayMqttObserver(relay2, mqtt));
        mqtt->attach("mqtt => relay-2", new MqttRelayObserver(relay2));
        button2->attach("button-2 => mqtt", new ButtonMqttObserver(button2, mqtt));

#ifdef HARDWARE_MONITORING
        hardwareMonitoring.attach("hardware-monitoring => mqtt", new HardwareMonitoringMqttObserver(mqtt));
#endif

#ifdef LIGHT_PIN
        light->attach(mqtt);
#endif

        return reconnect();
    }
    return false;
}

void publishResetReason()
{
    String reason = ESP.getResetReason();
    char buffer[50];
    reason.toCharArray(buffer, 50);
    mqtt->publish("device/reset", buffer);
}

int blinkDelay;

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
#ifdef DEBUG_BY_HTTP
        Logger.add(new HttpAppender(24));
#endif
        initMQTT();

        traceMemoryLeak("publish", &publishResetReason);

        initClock();

        blinkDelay = 1000;
    }
    else
    {
        randomSeed(analogRead(A0));
        int r = random(1000, 9999);
        String ssidAP = String("ESP8266-") + String(r);
        WifiAdapter.startAsAccessPoint(ssidAP.c_str());

        blinkDelay = 250;
    }

    telnetServer = new TelnetServer(23);
    telnetServer->add(new SetWifiCommand());
    telnetServer->add(new SetMqttCommand());
    telnetServer->add(new SetMqttTopicBaseCommand());
    telnetServer->add(new SetDeviceNameCommand());
    telnetServer->add(new SetRelayNameCommand());
    telnetServer->add(new SetButtonNameCommand());
    telnetServer->add(new UpdateFromOTACommand());
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
            mqtt->disconnect();
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

            String message;
            message.reserve(50);
            message.concat("{\"humidity\": ");
            message.concat(humidityAvg);
            message.concat(", \"temperature\": ");
            message.concat(temperatureAvg);
            message.concat("}");

            char buffer[50];
            message.toCharArray(buffer, 50);
            mqtt->publish("dht", buffer);

            humidity = 0;
            temperature = 0;
            dhtIndex = 0;
        }
    }
}
#endif

long lastBlink;
long lastDisconnect;
bool statusBlink = true;

void loop(void)
{
#ifdef HARDWARE_MONITORING
    hardwareMonitoring.process();
#endif

    traceFreeMemory();

    processTelnet();

    if (!WifiAdapter.isAccessPoint())
    {
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
    else
    {
        if (millis() > 5 * 60 * 1000) // 5 minutes
        {
            if (!telnetServer->active())
            {
                Serial.println("Reseting Access Point");
                Serial.flush();

                delay(10);
                ESP.reset();
            }
        }
    }

#ifdef LED_ACTIVITY
    if (lastBlink + blinkDelay < millis())
    {
        statusBlink = !statusBlink;
        digitalWrite(LED_ACTIVITY, statusBlink);
        lastBlink = millis();
    }
#endif

    delay(100);
}
