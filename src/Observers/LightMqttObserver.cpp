#ifndef LIGHT_MQTT_OBSERVER
#define LIGHT_MQTT_OBSERVER

#include "Logger.h"
#include "Observer.h"
#include "MqttAdapter.h"
#include "LightEventArgs.h"

class LightMqttObserver : public Observer<LightEventArgs>
{
  public:
    LightMqttObserver(MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
    }

    void notify(LightEventArgs args) override
    {
        DEBUGLN("publish light");
        _mqtt->publish("ldr", args.state);
    }

  private:
    MqttAdapter *_mqtt = NULL;
};

#endif