#include <Settings.h>
#include <EEPROM.h>
#include "Logger.h"

#define WIFI_START_1 0
#define WIFI_END_1 49
#define WIFI_START_2 50
#define WIFI_END_2 99
#define MQTT_CONNECTION_STRING_START 100
#define MQTT_CONNECTION_STRING_END 149
#define MQTT_TOPIC_BASE_START 150
#define MQTT_TOPIC_BASE_END 199
#define OTA_URL_START 200
#define OTA_URL_END 249
#define DEVICE_NAME_START 250
#define DEVICE_NAME_END 274

SettingsClass::SettingsClass()
{
	EEPROM.begin(512);
}

void SettingsClass::writeWifi(int index, char *value)
{
	if (index == 1)
		write(WIFI_START_1, WIFI_END_1, value);
	else
		write(WIFI_START_2, WIFI_END_2, value);
}

char *SettingsClass::readWifi(int index)
{
	if (index == 1)
		return read(WIFI_START_1, WIFI_END_1);
	else
		return read(WIFI_START_2, WIFI_END_2);
}

void SettingsClass::writeMqttConnectionString(char *value)
{
	write(MQTT_CONNECTION_STRING_START, MQTT_CONNECTION_STRING_END, value);
}

char *SettingsClass::readMqttConnectionString()
{
	return read(MQTT_CONNECTION_STRING_START, MQTT_CONNECTION_STRING_END);
}

void SettingsClass::writeMqttTopicBase(char *value)
{
	write(MQTT_TOPIC_BASE_START, MQTT_TOPIC_BASE_END, value);
}

char *SettingsClass::readMqttTopicBase()
{
	return read(MQTT_TOPIC_BASE_START, MQTT_TOPIC_BASE_END);
}

void SettingsClass::writeOtaUrl(char *value)
{
	write(OTA_URL_START, OTA_URL_END, value);
}

char *SettingsClass::readOtaUrl()
{
	return read(OTA_URL_START, OTA_URL_END);
}

void SettingsClass::writeDeviceName(char *value)
{
	write(DEVICE_NAME_START, DEVICE_NAME_END, value);
}

char *SettingsClass::readDeviceName()
{
	return read(DEVICE_NAME_START, DEVICE_NAME_END);
}

void SettingsClass::write(int from, int to, char *value)
{
	if (strlen(value) > to - from)
	{
		Logger.error(String("Length of '") + value + "' is bigger that " + String(to - from));
		return;
	}

	int i = 0;
	Logger.debug("Writing from " + String(from) + ": " + value);
	for (i = 0; i < strlen(value); ++i)
	{
		EEPROM.write(from + i, value[i]);
	}

	for (int j = from + i; j < to; j++)
		EEPROM.write(j, 0);

	EEPROM.commit();
}

char *SettingsClass::read(int from, int to)
{
	Logger.debug("Reading from " + String(from) + " to " + String(to) + ".");
	char *buffer = new char[to - from];

	int i;
	for (i = from; i < to - 1; ++i)
	{
		char c = char(EEPROM.read(i));
		buffer[i - from] = c;

		if (c == '\0')
			break;
	}

	if (i == to - 1)
		buffer[i - from] = '\0';

	Logger.debug("Value readed: " + String(buffer));
	return buffer;
}

SettingsClass Settings;