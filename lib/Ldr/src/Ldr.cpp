#include "Ldr.h"
#define LDR_READ_INTERVAL 30 * 1000

Ldr::Ldr(uint8_t pin)
{
	_pin = pin;
}

void Ldr::loop()
{
	if (_lastRead + LDR_READ_INTERVAL < millis())
	{
		int value = analogRead(_pin);
		DEBUG("LDR: ");
		DEBUGLN(value);

		LdrEventArgs args;
		args.value = value;

		notify(args);

		_lastRead = millis();
	}
}