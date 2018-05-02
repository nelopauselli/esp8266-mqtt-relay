# esp8266-mqtt-relay
firmware to handler relays (and DHT sensor) using mqtt message

## Features
* If Wifi isn't configure then start as AccessPoint.
* OTA update from a server.
* Telnet access to configure two wifis, mqtt server, ota url and names of pins (relay and button), easy to extend.
* DHT sensor support.

## Board tested
* NodeMCU
* Wemos D1 mini
* esp8266-esp01

## TODO
* remove dependency to NTPClient, replace by a countdown
* implement hardware monitoring (for example: free memory)
* implement GET Commands by each SET Command (don't show passwords)
* improve tests schema
* improve logger schema

## Known issues
* mqtt incoming messages are duplicate after reconnect to wifi