#ifndef LOGGER_CLASS
#define LOGGER_CLASS

#include <Arduino.h>
#include <Printable.h>

#include <Appenders/Appender.h>
#include <Appenders/Levels.h>

class Logger_t
{
  public:
	Logger_t();
	void cleanLog();
	void cleanDebug();
	void debugging(bool value);

	void add(Appender *appender);

	void trace(const String &message);
	void trace(const char message[]);
	void trace(const Printable &message);

	void debug(const String &message);
	void debug(const char message[]);
	void debug(const Printable &message);

	void error(const String &message);
	void error(const char message[]);
	void error(const Printable &message);

  private:
	bool _debugging;
	Appender *appenders[5];
	int appenderIndex = 0;
};

extern Logger_t Logger;

#endif
