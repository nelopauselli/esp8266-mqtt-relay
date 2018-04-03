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

		if (_mqtt != NULL)
			_mqtt->publish(_name, "state=on");
	}

	void off()
	{
		Logger.trace("OFF");
		if (_offAt != NULL)
			delete _offAt;
		_offAt = NULL;
		digitalWrite(_pin, HIGH);

		if (_mqtt != NULL)
			_mqtt->publish(_name, "state=off");
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

	char *toJSON()
	{
		bool state = !digitalRead(_pin);

		char *buffer = new char[255];
		strcpy(buffer, "{\"name\": \"");
		strcat(buffer, _name);
		strcat(buffer, "\", ");
		strcat(buffer, "\"actions\": [");
		if (state)
			strcat(buffer, "{\"name\": \"Apagar\", \"path\": \"/off\"}");
		else
			strcat(buffer, "{\"name\": \"Prender\", \"path\": \"/on\"}");
		strcat(buffer, "], ");
		strcat(buffer, "\"status\": \"");
		strcat(buffer, state ? "on" : "off");
		strcat(buffer, "\", ");
		strcat(buffer, "\"statusText\": ");
		if (state)
		{
			strcat(buffer, "\"se apagará a las ");
			if (_offAt != NULL)
				strcat(buffer, _offAt->toCharArray());
			strcat(buffer, " GMT\"");
		}
		else
		{
			strcat(buffer, "\"apagado\"");
		}
		strcat(buffer, "}");

		return buffer;
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