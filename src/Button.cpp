#ifndef BUTTON_CLASS
#define BUTTON_CLASS

#include <Arduino.h>
#include "Logger.h"

class Button
{
  public:
	Button(uint8_t pin)
	{
		_pin = pin;
		pinMode(_pin, INPUT_PULLUP);
		
		_lastPush = 0;
	}

	void onPressed(void (*callback)())
	{
		_onPressed = callback;
	}

	bool pushed()
	{
		if (_lastPush + 2000 > millis())
			return false; // if button was pushed in last 2 seconds, we return false

		bool state = !digitalRead(_pin);
		if (state)
		{
			_lastPush = millis();
			_onPressed();
		}
		
		return state;
	}

  private:
	uint8_t _pin;
	long _lastPush;
	void (*_onPressed)() = NULL;
};

#endif