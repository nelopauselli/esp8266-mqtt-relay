#include "MqttAdapter.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "Logger.h"

WiFiClient espClient;
PubSubClient client(espClient);

MqttAdapter::MqttAdapter(const char *server, int port, const char *topic, const char *name)
{
  DEBUGLN("Init Mqtt");

  _roottopic = new char[strlen(topic) + 1 + strlen(name) + 1 + 1];
  strcpy(_roottopic, topic);
  strcat(_roottopic, "/");
  strcat(_roottopic, name);
  strcat(_roottopic, "/");
  DEBUGLN(_roottopic);

  _name = name;
  DEBUGLN(_name);

  client.setServer(server, port);
}

void MqttAdapter::setCallback(MQTT_CALLBACK_SIGNATURE)
{
  client.setCallback(callback);
}

bool MqttAdapter::connect(const char *userName, const char *password)
{
  int count = 0;
  // Loop until we're reconnected
  while (!client.connected())
  {
    DEBUGLN("Attempting MQTT connection...");
    // Attempt to connect
    DEBUG("connecting using ");
    DEBUGLN(userName);

    if (client.connect(_name, userName, password))
    {
      DEBUGLN("connected");
    }
    else
    {
      DEBUG("[ERROR] failed, rc=");
      DEBUG(client.state());
      DEBUGLN(" try again in 5 seconds");

      if (client.state() == -2) //WIFi disconnected
        return false;

      // Wait 5 seconds before retrying
      delay(5000);
      count++;

      if (count > 5)
        return false;
    }
  }

  subscribeDeviceTopic();

  return true;
}

void MqttAdapter::subscribeDeviceTopic()
{
  char *topic = new char[strlen(_roottopic) + 2];
  strcpy(topic, _roottopic);
  strcat(topic, "#");

  DEBUG("subscribe to ");
  DEBUGLN(topic);
  client.subscribe(topic);
}

bool MqttAdapter::connected()
{
  return client.connected();
}

void MqttAdapter::disconnect()
{
  client.disconnect();
  espClient.flush();
  espClient.stop();
  espClient = WiFiClient();
}

void MqttAdapter::loop()
{
  client.loop();
}

void MqttAdapter::subscribe(const char *topic)
{
  client.subscribe(topic);
}

void MqttAdapter::publish(const char *subtopic, char *message)
{
  if (client.connected())
  {
    char *target = new char[strlen(_roottopic) + strlen(subtopic) + 1];
    strcpy(target, _roottopic);
    strcat(target, subtopic);

    DEBUG("Pubish [");
    DEBUG(target);
    DEBUG("] => ");
    DEBUGLN(message);

    client.publish(target, message);

    delay(10); //go message, go!

    delete target;
  }
}

const char *MqttAdapter::roottopic()
{
  return _roottopic;
}