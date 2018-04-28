#include "MqttAdapter.h"

#include <Logger.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

MqttAdapter::MqttAdapter(const char *server, int port, const char *topic, const char *name)
{
  Logger.trace("Init Mqtt");

  _roottopic = new char[strlen(topic) + 1 + strlen(name) + 1 + 1];
  strcpy(_roottopic, topic);
  strcat(_roottopic, "/");
  strcat(_roottopic, name);
  strcat(_roottopic, "/");
  Logger.debug(_roottopic);

  _name = name;
  Logger.debug(_name);

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
    Logger.trace("Attempting MQTT connection...");
    // Attempt to connect
    Logger.debug("connecting using " + String(userName));
    if (client.connect(_name, userName, password))
    {
      Logger.trace("connected");
    }
    else
    {
      Logger.error("failed, rc=" + String(client.state()) + " try again in 5 seconds");
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

bool MqttAdapter::connect()
{
  int count = 0;
  // Loop until we're reconnected
  while (!client.connected())
  {
    Logger.trace("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(_name))
    {
      Logger.trace("connected");
    }
    else
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        Logger.error("Connection to WiFi has been lost. Attempting to reconnect....");
        return false;
      }
      else
      {
        Logger.error("failed, rc=" + String(client.state()) + " try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
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

  Logger.trace(String("subscribe to ") + topic);
  client.subscribe(topic);
}

bool MqttAdapter::connected()
{
  return client.connected();
}

void MqttAdapter::loop()
{
  client.loop();
}

void MqttAdapter::subscribe(const char *topic)
{
  client.subscribe(topic);
}

void MqttAdapter::publish(const char *subtopic, const char *message)
{
  char *target = new char[strlen(_roottopic) + strlen(subtopic) + 1];
  strcpy(target, _roottopic);
  strcat(target, subtopic);

  client.publish(target, message);

  delay(10); //go message, go!

  delete target;
}

const char *MqttAdapter::roottopic()
{
  return _roottopic;
}