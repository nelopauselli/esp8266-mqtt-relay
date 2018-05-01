#ifndef APPENDER_CLASS
#define APPENDER_CLASS

class Appender
{
  public:
	virtual void write(const char *level, const char *message) = 0;
};

#endif