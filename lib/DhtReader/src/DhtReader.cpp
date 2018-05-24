#include "Arduino.h"
#include "DhtReader.h"
#include "Subject.h"
#include "DhtReadEventArgs.h"
#include <DHT.h>

DhtReader::DhtReader(uint8_t pin, DHT_TYPE type)
{
    _pin = pin;
    _type = type;
}

void DhtReader::loop()
{
    // Report every 2 seconds.
    if (millis() > _timeSinceLastRead + DHT_MINIMUM_SAMPLING_PERIOD)
    {
        uint32_t start = micros();
        int chk = _type == DHT_11
                      ? _dht.read11(_pin)
                      : _dht.read22(_pin);

        uint32_t stop = micros();

        _timeSinceLastRead = millis();

        switch (chk)
        {
        case DHTLIB_OK:
            DEBUG("DHT READ OK:\t");

            DEBUG("Humidity: ");
            DEBUG(_dht.humidity);
            DEBUG("%\tTemperature: ");
            DEBUG(_dht.temperature);
            DEBUGLN(" *C ");

            _humidity += _dht.humidity;
            _temperature += _dht.temperature;

            if (_count < DHT_MAX)
            {
                _count++;
            }
            else
            {
                float humidityAvg = _humidity / (_count + 1);
                float temperatureAvg = _temperature / (_count + 1);

                DhtReadEventArgs args;
                args.humidity = humidityAvg;
                args.temperature = temperatureAvg;

                Subject::notify(args);

                _humidity = 0;
                _temperature = 0;
                _count = 0;
            }

            break;
        case DHTLIB_ERROR_CHECKSUM:
            DEBUGLN("DHT READ Checksum error");
            break;
        case DHTLIB_ERROR_TIMEOUT:
            DEBUGLN("DHT READ Time out error");
            break;
        default:
            DEBUGLN("DHT READ Unknown error");
            break;
        }
    }
}
