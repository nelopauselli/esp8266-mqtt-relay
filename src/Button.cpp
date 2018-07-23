#ifndef BUTTON_CLASS
#define BUTTON_CLASS

#include <Arduino.h>
#include "ButtonEventArgs.h"
#include "Logger.h"
#include "MqttAdapter.h"
#include "Relay.cpp"

class Button : public Subject<ButtonEventArgs>
{
  public:
	Button(uint8_t pin, const char *name)
	{
		_pin = pin;
		_name = name;

		pinMode(_pin, INPUT_PULLUP);

		_lastPush = 0;
	}

	const char *name()
	{
		return _name;
	}

	void process()
	{
		if (_lastPush + 2000 > millis())
			return; // if button was pushed in last 2 seconds, we return

		bool state = !digitalRead(_pin);
		if (state)
		{
			_lastPush = millis();

			ButtonEventArgs args;
			args.pressed = true;
			Subject::notify(args);
		}
	}

  private:
	uint8_t _pin;
	const char *_name;

	unsigned long _lastPush;
};

#endif