#ifndef SETTINGS_CLASS
#define SETTINGS_CLASS

#include <Arduino.h>
#include <IPAddress.h>

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
	void writeRelayName(int index, char *value);
	char *readRelayName(int index);
	void writeButtonName(int index, char *value);
	char *readButtonName(int index);

	void writeHostAddress(IPAddress value);
	void readHostAddress(IPAddress& host);

	void writeHostPort(int value);
	int readHostPort();

  private:
	void write(int from, int to, char *value);
	char *read(int from, int to);
};

extern SettingsClass Settings;

#endif
