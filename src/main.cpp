#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define HARDWARE_MONITORING

#ifdef RELEASE
#define RELAY_DURATION 60
#define RELAY_NOTIFICATION_INTERVAL 300
#define OTA_INTERVAL 60 * 1000
#else
#define RELAY_DURATION 3
#define RELAY_NOTIFICATION_INTERVAL 10
#define OTA_INTERVAL 20 * 1000
#endif

extern "C"
{
#include "user_interface.h"
}

#include "Logger.h"

#include "MqttAdapter.h"
char *mqttUserName = NULL;
char *mqttPassword = NULL;

#include "Logger.h"

#include "TraceMemory.cpp"
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

#ifdef DHT_PIN
#include "DhtReader.h"
#include "Observers/DhtMqttObserver.cpp"
#ifdef DHT_TYPE_11
DhtReader dhtReader(DHT_PIN, DHT_11);
#else
DhtReader dhtReader(DHT_PIN, DHT_22);
#endif
#endif

Relay *relay1;
Relay *relay2;
#ifdef BUTTON1
Button *button1;
#endif
#ifdef BUTTON2
Button *button2;
#endif
#ifdef BUTTON3
Button *button3;
#endif
#ifdef LIGHT_PIN
#include "Observers/LightMqttObserver.cpp"
#include "Observers/MqttLightObserver.cpp"
Light *light;
#endif
#ifdef LDR_PIN
#include "Ldr.h"
#include "Observers/LdrMqttObserver.cpp"
Ldr ldr(LDR_PIN);
#endif

TelnetServer *telnetServer = NULL;
MqttAdapter *mqtt = NULL;

void initHardware()
{
#ifdef LED_ACTIVITY
    pinMode(LED_ACTIVITY, OUTPUT);
#endif

    DEBUGLN("Init relays...");

    relay1 = new Relay(RELAY1, Settings.readRelayName(1), RELAY_DURATION, RELAY_NOTIFICATION_INTERVAL);
    relay2 = new Relay(RELAY2, Settings.readRelayName(2), RELAY_DURATION, RELAY_NOTIFICATION_INTERVAL);

    DEBUGLN("Init buttons...");

#ifdef BUTTON1
    button1 = new Button(BUTTON1, Settings.readButtonName(1));
    button1->attach("button-1 => relay-1", new ButtonRelayObserver(relay1));
#endif

#ifdef BUTTON2
    button2 = new Button(BUTTON2, Settings.readButtonName(2));
    button2->attach("button-2 => relay-2", new ButtonRelayObserver(relay2));
#endif

#ifdef BUTTON3
    button3 = new Button(BUTTON3, Settings.readButtonName(3));
#endif

#ifdef LIGHT_PIN
    light = new Light(LIGHT_PIN, "luz");
#endif
}

/**
 * 
 * MQTT
 * 
 **/
void device_register()
{
    StaticJsonBuffer<2048> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    root["name"] = Settings.readDeviceName();
    root["topic"] = Settings.readMqttTopicBase();

    JsonArray &metrics = root.createNestedArray("metrics");

    JsonObject &metric0 = metrics.createNestedObject();
    metric0["name"] = "memory free";
    metric0["topic"] = "device/memoryfree";
    metric0["unit"] = "bytes";

#ifdef DHT_PIN
    JsonObject &metric1 = metrics.createNestedObject();
    metric1["name"] = "temperature";
    metric1["topic"] = "dht";
    metric1["property"] = "temperature";
    metric1["unit"] = "°C";

    JsonObject &metric2 = metrics.createNestedObject();
    metric2["name"] = "humidity";
    metric2["topic"] = "dht";
    metric2["property"] = "humidity";
    metric2["unit"] = "%";
#endif

#ifdef LDR_PIN
    JsonObject &metric3 = metrics.createNestedObject();
    metric3["name"] = "luz";
    metric3["topic"] = "ldr";
    metric3["unit"] = "L";
#endif

    JsonArray &components = root.createNestedArray("components");

    JsonObject &component0 = components.createNestedObject();
    component0["name"] = relay1->name();
    component0["topic"] = relay1->name();
    JsonArray &component0actions = component0.createNestedArray("actions");
    component0actions.add("turn on");
    component0actions.add("turn off");
    component0actions.add("+30m");
    component0actions.add("+1h");

    JsonObject &component1 = components.createNestedObject();
    component1["name"] = relay2->name();
    component1["topic"] = relay2->name();
    JsonArray &component1actions = component1.createNestedArray("actions");
    component1actions.add("turn on");
    component1actions.add("turn off");
    component1actions.add("+30m");
    component1actions.add("+1h");

#ifdef LIGHT_PIN
    JsonObject &component2 = components.createNestedObject();
    component2["name"] = light->name();
    component2["topic"] = light->name();
    JsonArray &component2actions = component2.createNestedArray("actions");
    component2actions.add("turn on");
    component2actions.add("turn off");
#endif

    root.prettyPrintTo(Serial);

    WiFiClient client;

    IPAddress host;
    Settings.readHostAddress(host);
    int port = Settings.readHostPort();
    DEBUG("Pubish device in ");
    DEBUG(host);
    DEBUG(":");
    DEBUGLN(port);

    if (client.connect(host, port))
    {
        client.println("POST /api/device HTTP/1.1");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(root.measureLength());
        client.println("Connection: close");
        client.println();

        root.printTo(client);

        client.flush();

        delay(10);
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    DEBUG("Message arrived [");
    DEBUG(topic);
    DEBUG("] <= ");

    char *message = new char[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    DEBUGLN(message);

    char *subtopic = new char[strlen(topic) - strlen(mqtt->roottopic()) + 1];
    strcpy(subtopic, topic + strlen(mqtt->roottopic()));

    MqttEventArgs args;
    args.topic = subtopic;
    args.payload = message;
    mqtt->notify(args);

    if (strcmp(topic, "/devices/search") == 0)
    {
        device_register();
    }

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
    DEBUGLN("connecting to mqtt with user & pass");
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
        DEBUG("Connecting to broker in ");
        DEBUG(mqttServer);
        DEBUG(":");
        DEBUGLN(mqttPort);

        char *topic = Settings.readMqttTopicBase();
        char *deviceName = Settings.readDeviceName();
        mqtt = new MqttAdapter(mqttServer, mqttPort, topic, deviceName);

        relay1->attach("relay-1 => mqtt", new RelayMqttObserver(relay1, mqtt));
        mqtt->attach("mqtt => relay-1", new MqttRelayObserver(relay1));
#ifdef BUTTON1
        button1->attach("button-1 => mqtt", new ButtonMqttObserver(button1, mqtt));
#endif

        relay2->attach("relay-2 => mqtt", new RelayMqttObserver(relay2, mqtt));
        mqtt->attach("mqtt => relay-2", new MqttRelayObserver(relay2));
#ifdef BUTTON2
        button2->attach("button-2 => mqtt", new ButtonMqttObserver(button2, mqtt));
#endif

#ifdef BUTTON3
        button3->attach("button-3 => mqtt", new ButtonMqttObserver(button3, mqtt));
#endif

#ifdef HARDWARE_MONITORING
        hardwareMonitoring.attach("hardware-monitoring => mqtt", new HardwareMonitoringMqttObserver(mqtt));
#endif
#ifdef DHT_PIN
        dhtReader.attach("dht => mqtt", new DhtMqttObserver(mqtt));
#endif
#ifdef LIGHT_PIN
        light->attach("light => mqtt", new LightMqttObserver(light, mqtt));
        mqtt->attach("mqtt => light", new MqttLightObserver(light));
#endif
#ifdef LDR_PIN
        ldr.attach("ldr => mqtt", new LdrMqttObserver(mqtt));
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

unsigned int blinkDelay = 500;

void setup()
{
#ifndef RELEASE
    Serial.begin(115200);
#endif

    traceFreeMemory();

    initHardware();

#ifdef LED_ACTIVITY
    digitalWrite(LED_ACTIVITY, LOW);
#endif

    DEBUG("ChipID: ");
    DEBUGLN(ESP.getChipId());

    // IPAddress host(192, 168, 1, 10);
    // Settings.writeHostAddress(host);
    // Settings.writeHostPort(80);

    WifiAdapter.addAP(Settings.readWifi(1));
    WifiAdapter.addAP(Settings.readWifi(2));

    if (WifiAdapter.connect())
    {
        initMQTT();

        traceMemoryLeak("publish", &publishResetReason);

        DEBUG("MAC: ");
        DEBUGLN(WiFi.macAddress());

        blinkDelay = 0;
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

    DEBUGLN("ready");

#ifdef LED_ACTIVITY
    digitalWrite(LED_ACTIVITY, HIGH);
#endif

    device_register();
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

unsigned long lastOTA = 0;
void loopOTA()
{
    if (lastOTA + OTA_INTERVAL < millis())
    {
        DEBUGLN("Searching updates");
        UpdateFromOTACommand cmd;
        cmd.update();

        DEBUGLN("no updates");
        lastOTA = millis();
    }
}

void processButtons()
{
#ifdef BUTTON1
    button1->process();
#endif
#ifdef BUTTON2
    button2->process();
#endif
#ifdef BUTTON3
    button3->process();
#endif
}

void processRelays()
{
    relay1->loop();
    relay2->loop();
}

#ifdef LIGHT_PIN
void processLights()
{
    light->loop();
}
#endif

void processTelnet()
{
    if (telnetServer != NULL)
        telnetServer->process();
}

unsigned long lastBlink;
long lastDisconnect;
bool statusBlink = true;

void loop(void)
{
#ifdef HARDWARE_MONITORING
    hardwareMonitoring.process();
#endif

    traceFreeMemory();

    processTelnet();

    traceMemoryLeak("processButtons", &processButtons);

    traceMemoryLeak("processRelays", &processRelays);
#ifdef LIGHT_PIN
    traceMemoryLeak("processLights", &processLights);
#endif

#ifdef DHT_PIN
    dhtReader.loop();
#endif

#ifdef LDR_PIN
    ldr.loop();
#endif

    if (!WifiAdapter.isAccessPoint())
    {
        loopMQTT();
        loopOTA();
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
    if (blinkDelay > 0 && lastBlink + blinkDelay < millis())
    {
        statusBlink = !statusBlink;
        digitalWrite(LED_ACTIVITY, statusBlink);
        lastBlink = millis();
    }
#endif

    delay(100);
}