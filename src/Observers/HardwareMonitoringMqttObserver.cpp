#ifndef HARDWARE_MONITORING_MQTT_OBSERVER_CLASS
#define HARDWARE_MONITORING_MQTT_OBSERVER_CLASS

#define HARDWARE_MONITORING_INTERVAL 10000

#include "Observer.h"
#include "HardwareMonitoringArgs.h"
#include "MqttAdapter.h"

class HardwareMonitoringMqttObserver : public Observer<HardwareMonitoringArgs>
{
  public:
    HardwareMonitoringMqttObserver(MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
    }

    void notify(HardwareMonitoringArgs args) override
    {
        if (_min > args.freeMemory)
            _min = args.freeMemory;

        if (_last + HARDWARE_MONITORING_INTERVAL < millis())
        {
            char memoryfree[16];
            itoa(_min, memoryfree, 10);
            _mqtt->publish("device/memoryfree", memoryfree);

            delay(10);

            _last = millis();
            _min = 0xFFFFFFFF;
        }
    }

  private:
    MqttAdapter *_mqtt = NULL;
    long _last = 0;
    uint32 _min = 0xFFFFFFFF;
};

#endif