#ifndef LIGHT_CLASS
#define LIGHT_CLASS

#include <Arduino.h>
#include "Logger.h"
#include "MqttAdapter.h"

class Light
{
  public:
	Light(uint8_t pin, const char *name)
	{
		_pin = pin;
		_name = name;

		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, HIGH);
	}

	const char *name()
	{
		return _name;
	}

	void toggle()
	{
		int state = !digitalRead(_pin);

		if (!state)
		{
			on();
		}
		else
		{
			off();
		}
	}

	void on()
	{
		DEBUG(_name);
		DEBUGLN(" ON");

		digitalWrite(_pin, LOW);
		publishState();
	}

	void off()
	{
		DEBUG(_name);
		DEBUG(" OFF");

		digitalWrite(_pin, HIGH);
		publishState();
	}

	void publishState()
	{
		if (_mqtt != NULL)
		{
			int state = !digitalRead(_pin);
			if (state)
			{
				_mqtt->publish(_name, "state on");
			}
			else
			{
				_mqtt->publish(_name, "state off");
			}
		}
	}

	void loop()
	{
	}

	void invoke(char *payload)
	{
		if (strcmp(payload, "turn on") == 0)
			on();
		else if (strcmp(payload, "turn off") == 0)
			off();
		else if (strcmp(payload, "state") == 0)
			publishState();
	}

	void attach(MqttAdapter *mqtt)
	{
		_mqtt = mqtt;
	}

  private:
	const char *_name;
	uint8_t _pin;
	MqttAdapter *_mqtt;
};

#endif