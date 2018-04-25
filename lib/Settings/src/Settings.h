#ifndef SETTINGS_CLASS
#define SETTINGS_CLASS

#include <Arduino.h>

class SettingsClass
{
public:
	SettingsClass();
	void writeWifi(int index, char *value);
	char *readWifi(int index);
	void writeMqttConnectionString(char *value);
	char *readMqttConnectionString();
	void writeMqttTopicBase(char *value);
	char *readMqttTopicBase();
	void writeOtaUrl(char *value);
	char *readOtaUrl();
	void writeDeviceName(char *value);
	char *readDeviceName();

private:
	void write(int from, int to, char *value);
	char *read(int from, int to);
};

extern SettingsClass Settings;

#endif
