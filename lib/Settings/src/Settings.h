#ifndef SETTINGS_CLASS
#define SETTINGS_CLASS

#include <Arduino.h>

class SettingsClass
{
  public:
	SettingsClass();
	void writeWifi(int index, String value);
	char *readWifi(int index);
	void writeMqttConnectionString(String value);
	char *readMqttConnectionString();
	void writeMqttTopicBase(String value);
	char *readMqttTopicBase();
	
	void writeOtaUrl(String value);
	char *readOtaUrl();

  private:
	void write(int from, int to, String value);
	char *read(int from, int to);
};

extern SettingsClass Settings;

#endif
