#ifndef MQTT_ADAPTER_CLASS
#define MQTT_ADAPTER_CLASS

#include <PubSubClient.h>

class MqttAdapter
{
public:
  MqttAdapter(const char *name, const char *server, int port);
  bool connect(const char *userName, const char *password);
  bool connect();
  void setCallback(MQTT_CALLBACK_SIGNATURE);
  bool connected();

  void subscribe(const char *topic);
  void publish(const char *subtopic, const char *message);

  void loop();

private:
  char *_name;
};

#endif // !MQTT_ADAPTER_CLASS
