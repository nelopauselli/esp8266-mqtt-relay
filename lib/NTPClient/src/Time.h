#ifndef TIME_CLASS
#define TIME_CLASS

#include <Arduino.h>

struct TimeSpan
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

class Time
{
public:
	Time(uint32_t time);
	char *toCharArray();
	void toCharArray(char *);
	void add(TimeSpan timespan);
	bool isLessThan(Time *other);
	uint32_t getUnixTime();

private:
	uint32_t _time;
};

#endif