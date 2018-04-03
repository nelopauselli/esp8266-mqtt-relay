#ifndef WIFI_ADAPTER_CLASS
#define WIFI_ADAPTER_CLASS

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Settings.h>
#include "Logger.h"

class WifiAdapter_t
{
  public:
	IPAddress getIP();
	void addAP(const char *ssid, const char *password);
	bool connect();
	void startAsAccessPoint(const char *ssid);
	int32_t getRSSI(const char *target_ssid);
	String listWifiAsJson();

  private:
	bool _accessPoint = false;
	ESP8266WiFiMulti _wifiMulti;
};

extern WifiAdapter_t WifiAdapter;

#endif