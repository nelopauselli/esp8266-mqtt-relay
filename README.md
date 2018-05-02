# esp8266-mqtt-relay
Firmware to handle relays, buttons and DHT sensor using mqtt message.

## Features
* Handle two relay with timer to turn off.
* Handle two buttons to turn on relay.
* Handle DHT sensor to report data.
* Handle LED night light.
* Handle activity LED.
* Mqtt integration.
* OTA update from a server.
* Telnet access to configure two wifis, mqtt server, ota url and names of pins (relay and button), easy to extend.
* If Wifi isn't configured or couldn't connect then start as Access Point.

## Board tested
* NodeMCU
* Wemos D1 mini
* esp8266-esp01 (without DHT and LEDs support)

## MQTT Broker tested
* Mosquitto (http://mosquitto.org/) on a Raspberry Pi
* cloudmqtt (https://www.cloudmqtt.com)

## Telnet console
![Telnet console](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/telnet-screenshot.png  "")

## Device sample
in progress...| complete | working
--- | --- | ---
![Device in progress](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/device-1.jpg  "") | ![Device complete](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/device-2.jpg  "") | ![Device working](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/device-3.jpg  "") 

## Dashboard
![Dashboard console](https://github.com/nelopauselli/esp8266-mqtt-relay/raw/master/doc/dashboard-1.png  "")

## TODO
* remove dependency to NTPClient, replace by a countdown
* implement hardware monitoring (for example: free memory)
* implement GET Commands by each SET Command (don't show passwords)
* improve tests schema
* improve logger schema
* design a PCB ?
* Make a generic firmware, then user should be configured the *pins* instead of *relays*, *buttons* ...

## Known issues
* mqtt incoming messages are duplicate after reconnect to wifi 