#ifndef DHT_READER_CLASS
#define DHT_READER_CLASS

#include "Arduino.h"
#include "Subject.h"
#include "DhtReadEventArgs.h"
#include <DHT.h>

#define DHT_MAX 5
#define DHT_MINIMUM_SAMPLING_PERIOD 2000

enum DHT_TYPE {
  DHT_11 = 11,
  DHT_22 = 22
};

class DhtReader : public Subject<DhtReadEventArgs>
{
  public:
    DhtReader(uint8_t pin, DHT_TYPE type);
    void loop();
  private:
    uint8_t _pin;
    dht _dht;
    DHT_TYPE _type;
    float _humidity;
    float _temperature;
    int _count = 0;

    unsigned long _timeSinceLastRead = 0;
};

#endif