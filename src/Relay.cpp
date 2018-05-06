#ifndef RELAY_CLASS
#define RELAY_CLASS

#include <Arduino.h>
#include "Logger.h"
#include "Subject.h"
#include "SoftClockTime.h"
#include "RelayEventArgs.h"

class Relay : public Subject<RelayEventArgs>
{
  public:
	Relay(uint8_t pin, const char *name, TimeSpan duration)
	{
		_pin = pin;
		_name = name;
		_duration = duration;

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
		_offAt->add(_duration);
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
		int state = !digitalRead(_pin);

		RelayEventArgs args;
		if (state)
		{
			char message[24];
			strcpy(message, "state on until ");
			strcat(message, _offAt->toCharArray());
			
			args.state = message;
		}
		else
		{
			args.state = "state off";
		}
		Subject::notify(args);
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
			if (_offAt != NULL)
				_offAt->add(TimeSpan{0, 30, 0});
			publishState();
		}
		else if (strcmp(payload, "+1h") == 0)
		{
			if (_offAt != NULL)
				_offAt->add(TimeSpan{1, 0, 0});
			publishState();
		}
	}

  private:
	const char *_name;
	uint8_t _pin;
	Time *_offAt = NULL;
	TimeSpan _duration;
};

#endif