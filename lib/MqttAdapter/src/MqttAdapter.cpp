#include "MqttAdapter.h"

#include <Logger.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

MqttAdapter::MqttAdapter(const char *name, const char *server, int port)
{
  _name = new char[strlen(name) + 1];
  strcpy(_name, name);

  client.setServer(server, port);
}

void MqttAdapter::setCallback(MQTT_CALLBACK_SIGNATURE){
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

  char *topic = new char[strlen(_name)+3];
  strcpy(topic, _name);
  strcat(topic, "/#");
  client.subscribe(topic);

  return true;
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
  char *target = new char[strlen(_name)+1+strlen(subtopic)+1];
  strcpy(target, _name);
  strcat(target, "/");
  strcat(target, subtopic);
  
  client.publish(target, message);
}
