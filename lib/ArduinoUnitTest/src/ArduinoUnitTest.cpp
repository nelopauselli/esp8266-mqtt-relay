#include "Arduino.h"

#include "ArduinoUnitTest.h"

void Assert_t::areEqual(char *expected, char *actual)
{
    if (strlen(expected) != strlen(actual) || strcmp(expected, actual) != 0)
    {
        _fails++;

        Serial.println("FAIL.");
        Serial.print("  Expected:\t");
        Serial.println(expected);
        Serial.print("  Actual:\t");
        Serial.println(actual);
    }
    else
    {
        _success++;
    }
};

void Assert_t::summary()
{
    Serial.print("Tests summary: ");
    Serial.print(_success);
    Serial.print(" test success. ");
    Serial.print(_fails);
    Serial.println(" test fail.");
}

Assert_t Assert;
