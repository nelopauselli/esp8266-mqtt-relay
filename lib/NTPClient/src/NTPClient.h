#ifndef NTP_CLIENT_CLASS
#define NTP_CLIENT_CLASS

#include <Arduino.h>

class NTPClient
{
  public:
	bool initClockFromServer();
};

#endif