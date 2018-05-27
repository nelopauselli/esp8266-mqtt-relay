#include "Ldr.h"

Ldr::Ldr(uint8_t pin)
{
	_pin = pin;
}

void Ldr::loop()
{
	int value = analogRead(_pin);

	if (_last != value)
	{
		LdrEventArgs args;
		args.value = value;

		notify(args);

		_last = value;
	}
}