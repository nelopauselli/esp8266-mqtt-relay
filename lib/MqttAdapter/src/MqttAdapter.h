#ifndef MQTT_ADAPTER_CLASS
#define MQTT_ADAPTER_CLASS

#include <PubSubClient.h>
#include "Subject.h"
#include "MqttEventArgs.h"

class MqttAdapter : public Subject<MqttEventArgs>
{
public:
  MqttAdapter(const char *server, int port, const char *topic, const char *name);
  bool connect(const char *userName, const char *password);
  void disconnect();

  void setCallback(MQTT_CALLBACK_SIGNATURE);
  bool connected();

  void subscribeDeviceTopic();
  void subscribe(const char *topic);
  void publish(const char *subtopic, const char *message);

  void loop();

  const char *roottopic();

private:
  const char *_name;
  char *_roottopic;
};

#endif // !MQTT_ADAPTER_CLASS
