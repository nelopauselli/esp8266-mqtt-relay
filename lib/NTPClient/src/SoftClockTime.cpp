#include "SoftClockTime.h"

void SoftTimeClock_t::reset(uint32_t unixTime)
{
	_time = unixTime;
	_time -= 3*3600; // GMT-3
	_offsetStart = millis();
}

Time *SoftTimeClock_t::now()
{
	return new Time(_time + (millis() - _offsetStart) / 1000);
}

SoftTimeClock_t SoftTimeClock;