#ifndef SETTINGS_CLASS
#define SETTINGS_CLASS

#include <Arduino.h>

class SettingsClass
{
public:
	SettingsClass();
	void writeSSID(int index, String value);
	char *readSSID(int index);
	void writePassword(int index, String value);
	char *readPassword(int index);
	void writeMqttConnectionString(String value);
	char *readMqttConnectionString();
	void writeMqttTopicBase(String value);
	char *readMqttTopicBase();

private:
	void write(int from, int to, String value);
	char *read(int from, int to);
};

extern SettingsClass Settings;

#endif
