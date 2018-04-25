#include "MqttAdapter.h"

#include <Logger.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

MqttAdapter::MqttAdapter(const char *server, int port, const char *topic, const char *name)
{
  Logger.trace("Init Mqtt");

  _fulltopic = new char[strlen(topic) + 1 + strlen(name) + 1 + 1];
  strcpy(_fulltopic, topic);
  strcat(_fulltopic, "/");
  strcat(_fulltopic, name);
  strcat(_fulltopic, "/");
  Logger.debug(_fulltopic);

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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    Logger.debug("connecting using " + String(userName));
    if (client.connect(_name, userName, password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(_name))
    {
      Serial.println("connected");
    }
    else
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Connection to WiFi has been lost. Attempting to reconnect....");
        //wifiManager.autoConnect(apName.c_str(), apPass.c_str());
        return false;
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
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
  char *topic = new char[strlen(_fulltopic) + 2];
  strcpy(topic, _fulltopic);
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
  char *target = new char[strlen(_fulltopic) + strlen(subtopic) + 1];
  strcpy(target, _fulltopic);
  strcat(target, subtopic);

  client.publish(target, message);
}

const char *MqttAdapter::fulltopic()
{
  return _fulltopic;
}