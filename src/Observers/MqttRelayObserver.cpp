#ifndef MQTT_RELAY_OBSERVER_CLASS
#define MQTT_RELAY_OBSERVER_CLASS

#include "Observer.h"
#include "MqttEventArgs.h"
#include "Relay.cpp"

class MqttRelayObserver : public Observer<MqttEventArgs>
{
  public:
    MqttRelayObserver(Relay *relay)
    {
        _relay = relay;
    }

    void notify(MqttEventArgs *args) override
    {
        if (strcmp(_relay->name(), args->topic) == 0)
            _relay->invoke(args->payload);
    }

  private:
    Relay *_relay;
};

#endif