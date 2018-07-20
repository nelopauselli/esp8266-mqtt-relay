#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WifiAdapter.h>

#include <Splitter.h>

IPAddress WifiAdapter_t::getIP()
{
	if (_accessPoint)
		return WiFi.softAPIP();
	else
		return WiFi.localIP();
}

void WifiAdapter_t::addAP(char *connectionString)
{
	if (strlen(connectionString) > 0)
	{
		Splitter splitter = Splitter(connectionString);

		char *ssid = splitter.getNextChunk('@');
		char *password = splitter.getNextChunk('\0');
		
		_wifiMulti.addAP(ssid, password);
	}
}

bool WifiAdapter_t::connect()
{
	DEBUGLN("WiFi configured. connecting...");

	WiFi.mode(WIFI_STA);
	int i = 0;
	wl_status_t status;
	do
	{
		status = _wifiMulti.run();
		DEBUG("Status: ");
		DEBUGLN(status);

		if (status != WL_CONNECTED)
		{
			if (i == 20)
			{
				DEBUG("[ERROR] Connection fail. Status: ");
				DEBUGLN(status);
				return false;
			}

			delay(500);
			i++;
		}
	} while (status != WL_CONNECTED);

	DEBUG("Connection established with ");
	DEBUGLN(WiFi.SSID());
	DEBUG("RSSI: ");
	DEBUGLN(getRSSI(WiFi.SSID().c_str()));

	_accessPoint = false;

	DEBUG("IP address: ");
	DEBUGLN(getIP());

	return true;
}

void WifiAdapter_t::disconnect()
{
	WiFi.disconnect(true);

	int count = 0;
	DEBUGLN("disconnecting");
	while (WiFi.status() != WL_DISCONNECTED)
	{
		count++;
		if (count > 20)
			return;
		delay(100);
	}

	if (WiFi.status() == WL_DISCONNECTED)
		DEBUGLN("disconnected");
	else
		DEBUGLN("[ERROR] could not disconnect");
}

void WifiAdapter_t::startAsAccessPoint(const char *ssid)
{
	WiFi.mode(WIFI_AP);
	WiFi.softAP(ssid);

	DEBUG("Access Point \"");
	DEBUG(ssid);
	DEBUGLN("\" started");

	_accessPoint = true;

	DEBUG("IP address: ");
	DEBUGLN(getIP());
}

// Return RSSI or 0 if target SSID not found
int32_t WifiAdapter_t::getRSSI(const char *target_ssid)
{
	byte available_networks = WiFi.scanNetworks();

	for (int network = 0; network < available_networks; network++)
	{
		if (strcmp(WiFi.SSID(network).c_str(), target_ssid) == 0)
		{
			return WiFi.RSSI(network);
		}
	}
	return 0;
}

String WifiAdapter_t::listWifiAsJson()
{
	byte available_networks = WiFi.scanNetworks();

	String response = "[";
	for (int network = 0; network < available_networks; network++)
	{
		if (network > 0)
			response += ",";
		response += "{\"ssid\":\"" + WiFi.SSID(network) + "\",\"rssi\":\"" + String(WiFi.RSSI(network)) + "\"}";
	}
	response += "]";
	return response;
}

bool WifiAdapter_t::isAccessPoint()
{
	return _accessPoint;
}
WifiAdapter_t WifiAdapter;
