#ifndef RELAY_CLASS
#define RELAY_CLASS

#include <Arduino.h>
#include "Logger.h"
#include "Subject.h"
#include "RelayEventArgs.h"

#define SECOND 1000
#define MINUTE 60 * SECOND

class Relay : public Subject<RelayEventArgs>
{
  public:
	Relay(uint8_t pin, const char *name, unsigned long durationInMinutes, unsigned long notificationIntervalInSeconds)
	{
		_pin = pin;
		_name = name;
		_startAt = 0L;
		_duration = durationInMinutes * MINUTE;
		_notificationInterval = notificationIntervalInSeconds * SECOND;

		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, HIGH);
	}

	const char *name()
	{
		return _name;
	}

	unsigned long getTimeLeft()
	{
		if (_startAt == 0L)
			return 0;

		unsigned long endAt = _startAt + _duration + _extra;
		if (endAt < millis())
			return 0;

		return endAt - millis();
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

		_startAt = millis();

		digitalWrite(_pin, LOW);

		publishState();
	}

	void off()
	{
		DEBUG(_name);
		DEBUGLN(" OFF");

		_startAt = 0L;

		digitalWrite(_pin, HIGH);

		publishState();
	}

	void publishState()
	{
		int state = !digitalRead(_pin);

		RelayEventArgs args;
		if (state)
		{
			unsigned long secondsLeft = getTimeLeft() / 1000;
			if (secondsLeft < 120)
			{
				char buffer[10];
				ltoa(secondsLeft, buffer, 10);

				char message[24];
				strcpy(message, "turn off in ");
				strcat(message, buffer);
				strcat(message, " seconds");

				args.state = message;
			}
			else
			{
				unsigned long minutesLeft = secondsLeft / 60;

				char buffer[10];
				ltoa(minutesLeft, buffer, 10);

				char message[24];
				strcpy(message, "state on for ");
				strcat(message, buffer);
				strcat(message, " minutes");

				args.state = message;
			}
		}
		else
		{
			args.state = "state off";
		}
		Subject::notify(args);
	}

	void loop()
	{
		if (_startAt != 0L)
		{
			unsigned long timeLeft = getTimeLeft();

			DEBUG(_name);
			DEBUG(" turn off in ");
			DEBUGLN(timeLeft);

			if (timeLeft == 0)
			{
				off();
			}
			else if (_lastNotification + _notificationInterval < millis())
			{
				publishState();
				_lastNotification = millis();
			}
		}
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
			if (_startAt != 0)
				_extra += 30 * MINUTE;

			publishState();
		}
		else if (strcmp(payload, "+1h") == 0)
		{
			if (_startAt != 0)
				_extra += 60 * MINUTE;

			publishState();
		}
	}

  private:
	const char *_name;
	uint8_t _pin;
	unsigned long _startAt = 0L;
	unsigned long _duration;
	unsigned long _extra;

	unsigned long _lastNotification = 0L;
	unsigned long _notificationInterval;
};

#endif