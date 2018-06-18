#ifndef LDR_CLASS
#define LDR_CLASS

#include <Arduino.h>
#include "Subject.h"
#include "LdrEventArgs.h"

class Ldr : public Subject<LdrEventArgs>
{
  public:
	Ldr(uint8_t pin);
	void loop();

  private:
	uint8_t _pin;
	unsigned long _lastRead = -1;
};

#endif