#ifndef RELAY_CLASS
#define RELAY_CLASS

#include <Arduino.h>
#include "Logger.h"
#include "MqttAdapter.h"
#include "SoftClockTime.h"

class Relay
{
  public:
	Relay(uint8_t pin, const char *name)
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
		Logger.trace("ON");
		if (_offAt != NULL)
			delete _offAt;

		_offAt = SoftTimeClock.now();
		_offAt->add(TimeSpan{0, 0, 10});
		digitalWrite(_pin, LOW);

		publishState();
	}

	void off()
	{
		Logger.trace("OFF");
		if (_offAt != NULL)
			delete _offAt;
		_offAt = NULL;
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
				char *message = new char[255];
				strcpy(message, "state on until ");
				strcat(message, _offAt->toCharArray());
				_mqtt->publish(_name, message);
			}
			else
			{
				_mqtt->publish(_name, "state off");
			}
		}
	}

	bool process()
	{
		bool ret = false;

		Logger.debug("Processing " + String(_name));

		if (_offAt != NULL)
		{
			Time *now = SoftTimeClock.now();

			char buffer1[9];
			_offAt->toCharArray(buffer1);
			char buffer2[9];
			now->toCharArray(buffer2);
			Logger.debug(String(_name) + " turn off at " + buffer1 + ". Now is " + buffer2);

			if (_offAt->isLessThan(now))
			{
				off();
				ret = true;
			}
			delete now;
		}
		return ret;
	}

	void invoke(char *payload)
	{
		if (strcmp(payload, "turn on") == 0)
			on();
		else if (strcmp(payload, "turn off") == 0)
			off();
		else if (strcmp(payload, "state") == 0)
			publishState();
		else if (strcmp(payload, "+30m") == 0)
		{
			_offAt->add(TimeSpan{0, 0, 30});
			publishState();
		}
		else if (strcmp(payload, "+1h") == 0)
		{
			_offAt->add(TimeSpan{0, 1, 00});
			publishState();
		}
	}

	void attach(MqttAdapter *mqtt)
	{
		_mqtt = mqtt;
	}

  private:
	const char *_name;
	uint8_t _pin;
	Time *_offAt = NULL;
	MqttAdapter *_mqtt;
};

#endif