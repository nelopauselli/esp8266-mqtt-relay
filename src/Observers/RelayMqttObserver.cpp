#ifndef RELAY_MQTT_OBSERVER_CLASS
#define RELAY_MQTT_OBSERVER_CLASS

#include "Observer.h"
#include "RelayEventArgs.h"
#include "Relay.cpp"
#include "MqttAdapter.h"

class RelayMqttObserver : public Observer<RelayEventArgs>
{
  public:
    RelayMqttObserver(Relay *relay, MqttAdapter *mqtt)
    {
        _mqtt = mqtt;

        _topic = new char[strlen(relay->name()) + 2];
        strcpy(_topic, "/");
        strcat(_topic, relay->name());
    }

    void notify(RelayEventArgs args) override
    {
        _mqtt->publish(_topic, args.state);
    }

  private:
    MqttAdapter *_mqtt = NULL;
    char *_topic = NULL;
};

#endif