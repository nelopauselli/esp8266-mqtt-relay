# esp8266-mqtt-relay
firmware to handler relays (and DHT sensor) using mqtt message

## Features
* Handle two relay.
* Handle two buttons.
* Handle DHT sensor optional.
* Handle LED light optional.
* Handle LED acitivy optional.
* Mqtt integration optional.
* OTA update from a server.
* Telnet access to configure two wifis, mqtt server, ota url and names of pins (relay and button), easy to extend.
* If Wifi isn't configured or couldn't connect then start as Access Point.
* DHT sensor support.

## Board tested
* NodeMCU
* Wemos D1 mini
* esp8266-esp01

## MQTT Broker tested
* Mosquitto (http://mosquitto.org/) on a Raspberry Pi
* cloudmqtt (https://www.cloudmqtt.com)

## Telnet server
[Telnet console](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/telent-screenshot.png  "")

## TODO
* remove dependency to NTPClient, replace by a countdown
* implement hardware monitoring (for example: free memory)
* implement GET Commands by each SET Command (don't show passwords)
* improve tests schema
* improve logger schema

## Known issues
* mqtt incoming messages are duplicate after reconnect to wifi