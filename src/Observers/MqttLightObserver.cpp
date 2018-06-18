#ifndef MQTT_LIGHT_OBSERVER_CLASS
#define MQTT_LIGHT_OBSERVER_CLASS

#include "Observer.h"
#include "MqttEventArgs.h"
#include "Light.cpp"

class MqttLightObserver : public Observer<MqttEventArgs>
{
  public:
    MqttLightObserver(Light *light)
    {
        _light = light;
    }

    void notify(MqttEventArgs args) override
    {
        if (strcmp(_light->name(), args.topic) == 0)
            _light->invoke(args.payload);
    }

  private:
    Light *_light;
};

#endif