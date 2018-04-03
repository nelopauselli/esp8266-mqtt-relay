#include "Arduino.h"
#include "Logger.h"

extern "C" {
#include "user_interface.h"
}

void traceMemoryLeak(void (*callback)())
{
    uint32_t freeMemoryAtStart = system_get_free_heap_size();

    callback();

    uint32_t freeMemoryAtEnd = system_get_free_heap_size();

    if (freeMemoryAtEnd < freeMemoryAtStart)
    {
        Logger.error("Lost memory: " + String(freeMemoryAtStart - freeMemoryAtEnd) + " bytes.");
    }
}

uint32_t lastFreeMemory = 0;
uint32_t freeMemory = 0;

void traceFreeMemory()
{
    freeMemory = system_get_free_heap_size();
    if (lastFreeMemory != freeMemory)
    {
        Logger.debug("Free Memory: " + String(freeMemory) + " bytes");
        lastFreeMemory = freeMemory;
    }
}