#ifndef BUTTON_RELAY_OBSERVER_CLASS
#define BUTTON_RELAY_OBSERVER_CLASS

#include "Observer.h"
#include "ButtonEventArgs.h"
#include "Relay.cpp"

class ButtonRelayObserver : public Observer<ButtonEventArgs>
{
  public:
    ButtonRelayObserver(Relay *relay)
    {
        _relay = relay;
    }

    void notify(ButtonEventArgs *args) override
    {
        if (args->pressed)
            _relay->toggle();
    }

  private:
    Relay *_relay;
};

#endif