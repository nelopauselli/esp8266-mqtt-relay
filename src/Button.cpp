#ifndef BUTTON_CLASS
#define BUTTON_CLASS

#include <Arduino.h>
#include "Logger.h"
#include "MqttAdapter.h"
#include "Relay.cpp"

class Button
{
  public:
	Button(uint8_t pin, const char *name)
	{
		_pin = pin;
		_name = name;
		
		pinMode(_pin, INPUT_PULLUP);

		_lastPush = 0;
	}

	void process()
	{
		if (_lastPush + 2000 > millis())
			return; // if button was pushed in last 2 seconds, we return

		bool state = !digitalRead(_pin);
		if (state)
		{
			_lastPush = millis();

			if (_mqtt != NULL)
				_mqtt->publish(_name, "pressed");

			if (_relay != NULL)
				_relay->toggle();
		}
	}

	void attach(MqttAdapter *mqtt)
	{
		_mqtt = mqtt;
	}

	void attach(Relay *relay)
	{
		_relay = relay;
	}

  private:
	uint8_t _pin;
	const char *_name;

	long _lastPush;

	Relay *_relay;
	MqttAdapter *_mqtt;
};

#endif