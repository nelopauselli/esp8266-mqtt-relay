#include "Time.h"

Time::Time(uint32_t time)
{
	_time = time;
}

void Time::add(TimeSpan timespan)
{
	_time += timespan.hour*3600 + timespan.minute * 60 + timespan.second;
}

char *Time::toCharArray()
{
	char *buffer = new char[9];
	toCharArray(buffer);
	return buffer;
}

void Time::toCharArray(char *buffer)
{
	int seconds = _time % 60;
	int minutes = _time / 60 % 60;
	int hours = _time / 3600 % 24;

	if (hours < 10)
		buffer[0] = '0';
	else
		buffer[0] = (hours / 10 % 10) + 48;
	buffer[1] = (hours % 10) + 48;

	buffer[2] = ':';

	if (hours < 10)
		buffer[3] = '0';
	else
		buffer[3] = (minutes / 10 % 10) + 48;
	buffer[4] = (minutes % 10) + 48;

	buffer[5] = ':';

	if (hours < 10)
		buffer[6] = '0';
	else
		buffer[6] = (seconds / 10 % 10) + 48;
	buffer[7] = (seconds % 10) + 48;

	buffer[8] = '\0';
}

bool Time::isLessThan(Time *other)
{
	return (_time < other->getUnixTime());
}

uint32_t Time::getUnixTime(){
	return _time;
}