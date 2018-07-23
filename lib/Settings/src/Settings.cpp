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
#define RELAY_NAME_START_1 275
#define RELAY_NAME_END_1 289
#define RELAY_NAME_START_2 290
#define RELAY_NAME_END_2 304
#define BUTTON_NAME_START_1 305
#define BUTTON_NAME_END_1 319
#define BUTTON_NAME_START_2 320
#define BUTTON_NAME_END_2 334
#define HOST_ADDRESS_START 335
#define HOST_ADDRESS_END 338
#define HOST_PORT_START 339
#define HOST_PORT_END 340

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

void SettingsClass::writeRelayName(int index, char *value)
{
	if (index == 1)
		write(RELAY_NAME_START_1, RELAY_NAME_END_1, value);
	else
		write(RELAY_NAME_START_2, RELAY_NAME_END_2, value);
}

char *SettingsClass::readRelayName(int index)
{
	if (index == 1)
		return read(RELAY_NAME_START_1, RELAY_NAME_END_1);
	else
		return read(RELAY_NAME_START_2, RELAY_NAME_END_2);
}

void SettingsClass::writeButtonName(int index, char *value)
{
	if (index == 1)
		write(BUTTON_NAME_START_1, BUTTON_NAME_END_1, value);
	else
		write(BUTTON_NAME_START_2, BUTTON_NAME_END_2, value);
}

char *SettingsClass::readButtonName(int index)
{
	if (index == 1)
		return read(BUTTON_NAME_START_1, BUTTON_NAME_END_1);
	else
		return read(BUTTON_NAME_START_2, BUTTON_NAME_END_2);
}

void SettingsClass::writeHostAddress(IPAddress value)
{
	EEPROM.write(HOST_ADDRESS_START, value[0]);
	EEPROM.write(HOST_ADDRESS_START + 1, value[1]);
	EEPROM.write(HOST_ADDRESS_START + 2, value[2]);
	EEPROM.write(HOST_ADDRESS_START + 3, value[3]);

	EEPROM.commit();
}

void SettingsClass::readHostAddress(IPAddress& host)
{
	host[0] = EEPROM.read(HOST_ADDRESS_START);
	host[1] = EEPROM.read(HOST_ADDRESS_START + 1);
	host[2] = EEPROM.read(HOST_ADDRESS_START + 2);
	host[3] = EEPROM.read(HOST_ADDRESS_START + 3);
}

void SettingsClass::writeHostPort(int value)
{
	byte two = (value & 0xFF);
	byte one = ((value >> 8) & 0xFF);

	EEPROM.write(HOST_PORT_START, two);
	EEPROM.write(HOST_PORT_START + 1, one);

	EEPROM.commit();
}

int SettingsClass::readHostPort()
{
	int two = EEPROM.read(HOST_PORT_START);
	int one = EEPROM.read(HOST_PORT_START + 1);

	return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

void SettingsClass::write(int from, int to, char *value)
{
	if (strlen(value) > to - from)
	{
		DEBUG("[ERROR] Length of '");
		DEBUG(value);
		DEBUG("' is bigger that ");
		DEBUGLN(to - from);

		return;
	}

	int i = 0;
	DEBUG("Writing from ");
	DEBUG(from);
	DEBUG(": ");
	DEBUGLN(value);

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
	DEBUG("Reading from ");
	DEBUG(from);
	DEBUG(" to ");
	DEBUG(to);
	DEBUGLN(".");

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

	DEBUG("Value readed: ");
	DEBUGLN(buffer);

	return buffer;
}

SettingsClass Settings;