#ifndef DHT_MQTT_OBSERVER
#define DHT_MQTT_OBSERVER

#include "Logger.h"
#include "Observer.h"
#include "MqttAdapter.h"
#include "DhtReadEventArgs.h"

class DhtMqttObserver : public Observer<DhtReadEventArgs>
{
  public:
    DhtMqttObserver(MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
    }

    void notify(DhtReadEventArgs args) override
    {
        DEBUGLN("publish dht");
        String message;
        message.reserve(50);
        message.concat("{\"humidity\": ");
        message.concat(args.humidity);
        message.concat(", \"temperature\": ");
        message.concat(args.temperature);
        message.concat("}");
        char buffer[50];
        message.toCharArray(buffer, 50);
        
        _mqtt->publish("dht", buffer);
    }

  private:
    MqttAdapter *_mqtt = NULL;
};

#endif