#ifndef SOFT_CLOCK_TIME_CLASS
#define SOFT_CLOCK_TIME_CLASS

#include <Arduino.h>
#include "Time.h"

class SoftTimeClock_t
{
  public:
	void reset(uint32_t unixTime);
	Time *now();

  private:
	uint32_t _time;
	long _offsetStart;
};

extern SoftTimeClock_t SoftTimeClock;

#endif