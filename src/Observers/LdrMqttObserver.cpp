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
        String message;
        message.reserve(50);
        message.concat("{\"value\": ");
        message.concat(args.value);
        message.concat("}");
        char buffer[50];
        message.toCharArray(buffer, 50);
        
        _mqtt->publish("ldr", buffer);
    }

  private:
    MqttAdapter *_mqtt = NULL;
};

#endif