#include <Settings.h>
#include <EEPROM.h>
#include "Logger.h"

#define WIFI_SSID_START_1 0
#define WIFI_SSID_END_1 31
#define WIFI_PASSWORD_START_1 32
#define WIFI_PASSWORD_END_1 96
#define WIFI_SSID_START_2 97
#define WIFI_SSID_END_2 128
#define WIFI_PASSWORD_START_2 129
#define WIFI_PASSWORD_END_2 193
#define MQTT_CONNECTION_STRING_START 194
#define MQTT_CONNECTION_STRING_END 254
#define MQTT_TOPIC_BASE_START 255
#define MQTT_TOPIC_BASE_END 275

SettingsClass::SettingsClass()
{
	EEPROM.begin(512);
}

void SettingsClass::writeSSID(int index, String value)
{
	if (index == 1)
		write(WIFI_SSID_START_1, WIFI_SSID_END_1, value);
	else
		write(WIFI_SSID_START_2, WIFI_SSID_END_2, value);
}

char *SettingsClass::readSSID(int index)
{
	if (index == 1)
		return read(WIFI_SSID_START_1, WIFI_SSID_END_1);
	else
		return read(WIFI_SSID_START_2, WIFI_SSID_END_2);
}

void SettingsClass::writePassword(int index, String value)
{
	if (index == 1)
		write(WIFI_PASSWORD_START_1, WIFI_PASSWORD_END_1, value);
	else
		write(WIFI_PASSWORD_START_2, WIFI_PASSWORD_END_2, value);
}

char *SettingsClass::readPassword(int index)
{
	if (index == 1)
		return read(WIFI_PASSWORD_START_1, WIFI_PASSWORD_END_1);
	else
		return read(WIFI_PASSWORD_START_2, WIFI_PASSWORD_END_2);
}

void SettingsClass::writeMqttConnectionString(String value)
{
	write(MQTT_CONNECTION_STRING_START, MQTT_CONNECTION_STRING_END, value);
}

char *SettingsClass::readMqttConnectionString()
{
	return read(MQTT_CONNECTION_STRING_START, MQTT_CONNECTION_STRING_END);
}

void SettingsClass::writeMqttTopicBase(String value)
{
	write(MQTT_TOPIC_BASE_START, MQTT_TOPIC_BASE_END, value);
}

char *SettingsClass::readMqttTopicBase()
{
	return read(MQTT_TOPIC_BASE_START, MQTT_TOPIC_BASE_END);
}

void SettingsClass::write(int from, int to, String value)
{
	if (value.length() > to - from)
	{
		Logger.error("Length of '" + value + "' is bigger that " + String(to - from));
		return;
	}

	int i = 0;
	Logger.debug("Writing from " + String(from) + ": " + value);
	for (i = 0; i < value.length(); ++i)
	{
		EEPROM.write(from + i, value[i]);
	}

	for (int j = from + i; j < to; j++)
		EEPROM.write(j, 0);

	EEPROM.commit();
}

char *SettingsClass::read(int from, int to)
{
	Logger.debug("Reading from " + String(from) + ".");
	char *buffer = new char[to-from];
	for (int i = from; i < to; ++i)
	{
		char c = char(EEPROM.read(i));
		buffer[i-from] = c;

		if (c == '\0')
			break;
	}

	Logger.debug("Value readed: " + String(buffer));
	return buffer;
}

SettingsClass Settings;