#ifndef LIGHT_MQTT_OBSERVER
#define LIGHT_MQTT_OBSERVER

#include "Logger.h"
#include "Observer.h"
#include "MqttAdapter.h"
#include "Light.cpp"
#include "LightEventArgs.h"

class LightMqttObserver : public Observer<LightEventArgs>
{
  public:
    LightMqttObserver(Light *light, MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
        _topic = light->name();
    }

    void notify(LightEventArgs args) override
    {
        _mqtt->publish(_topic, args.state);
    }

  private:
    MqttAdapter *_mqtt = NULL;
    const char *_topic = NULL;
};

#endif