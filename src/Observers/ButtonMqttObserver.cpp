#ifndef BUTTON_MQTT_OBSERVER_CLASS
#define BUTTON_MQTT_OBSERVER_CLASS

#include "Observer.h"
#include "ButtonEventArgs.h"
#include "Button.cpp"
#include "MqttAdapter.h"

class ButtonMqttObserver : public Observer<ButtonEventArgs>
{
  public:
    ButtonMqttObserver(Button *button, MqttAdapter *mqtt)
    {
        _mqtt = mqtt;
        _topic = button->name();
    }

    void notify(ButtonEventArgs args) override
    {
        if (args.pressed)
            _mqtt->publish(_topic, "button pressed");
    }

  private:
    MqttAdapter *_mqtt = NULL;
    const char *_topic = NULL;
};

#endif