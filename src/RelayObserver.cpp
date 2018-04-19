#ifndef RELAY_OBSERVER_CLASS
#define RELAY_OBSERVER_CLASS

#include "Observer.h"
#include "MqttEventArgs.h"
#include "Relay.cpp"

class RelayObserver : public Observer<MqttEventArgs>
{
  public:
    RelayObserver(Relay *relay)
    {
        _relay = relay;
        _topic = new char[strlen(relay->name()) + 2];
        strcpy(_topic, "/");
        strcat(_topic, relay->name());
    }

    void notify(MqttEventArgs *args) override
    {
        if (strcmp(_topic, args->topic) == 0)
            _relay->invoke(args->payload);
    }

  private:
    Relay *_relay;
    char *_topic;
};

#endif