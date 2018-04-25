#ifndef MQTT_EVENT_ARGS_CLASS
#define MQTT_EVENT_ARGS_CLASS

struct MqttEventArgs
{
  char *topic;
  char *payload;
};

#endif