#ifndef LDR_MQTT_OBSERVER
#define LDR_MQTT_OBSERVER

#include "Logger.h"
#include "Observer.h"
#include "MqttAdapter.h"
#include "LdrEventArgs.h"

class LdrMqttObserver : public Observer<LdrEventArgs>
{
  public:
    LdrMqttObserver(MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
    }

    void notify(LdrEventArgs args) override
    {
        DEBUGLN("publish ldr");
       
        char buffer[4];
        itoa(args.value, buffer, 10);
        
        _mqtt->publish("ldr", buffer);
    }

  private:
    MqttAdapter *_mqtt = NULL;
};

#endif