#ifndef MQTT_EVENT_ARGS_CLASS
#define MQTT_EVENT_ARGS_CLASS

class MqttEventArgs
{
public:
  MqttEventArgs(char *topic, char *payload)
      : topic(topic), payload(payload)
  {
  }
  char *topic;
  char *payload;
};

#endif