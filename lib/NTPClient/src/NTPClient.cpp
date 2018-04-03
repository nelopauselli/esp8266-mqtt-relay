#include "NTPClient.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "SoftClockTime.h"
#include "Logger.h"

bool NTPClient::initClockFromServer()
{
	const char *NTPServerName = "3.ar.pool.ntp.org";

	const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message
	char NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

	Logger.debug("Starting UDP");

	WiFiUDP UDP;
	UDP.begin(123); // Start listening for UDP messages on port 123
	Logger.debug("Local port:\t" + String(UDP.localPort()));

	IPAddress timeServerIP; // time.nist.gov NTP server address
	if (!WiFi.hostByName(NTPServerName, timeServerIP))
	{
		Logger.error("DNS lookup failed. Rebooting.");
		return false;
	}
	Logger.debug("Time server IP:\t" + String(timeServerIP));

	Logger.debug("Sending NTP request.");

	// Send an NTP request
	memset(NTPBuffer, 0, NTP_PACKET_SIZE); // set all bytes in the buffer to 0
	// Initialize values needed to form NTP request
	NTPBuffer[0] = 0b11100011; // LI, Version, Mode
	// send a packet requesting a timestamp:
	UDP.beginPacket(timeServerIP, 123); // NTP requests are to port 123
	UDP.write(NTPBuffer, NTP_PACKET_SIZE);
	UDP.endPacket();

	int count = 0;
	while (UDP.parsePacket() == 0)
	{
		delay(100);
		count++;
		if (count > 50)
		{
			Logger.error("Timeout!");
			return false;
		}
	}

	UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
	// Combine the 4 timestamp bytes into one 32-bit number
	uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
	// Convert NTP time to a UNIX timestamp:
	// Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
	const uint32_t seventyYears = 2208988800UL;
	// subtract seventy years:
	uint32_t UNIXTime = NTPTime - seventyYears;
	Logger.debug("Time as UNIX format is: " + String(UNIXTime));

	SoftTimeClock.reset(UNIXTime);

	return true;
}
