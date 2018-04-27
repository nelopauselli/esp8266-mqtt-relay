#include "Arduino.h"
#include "Logger.h"

#define DEBUG_MEMORY

extern "C" {
#include "user_interface.h"
}

void debugMemory()
{
    Serial.print("[MEMORY],");
    Serial.println(system_get_free_heap_size());
}

void traceMemoryLeak(const char *name, void (*callback)())
{
    uint32 freeMemoryAtStart = system_get_free_heap_size();

    callback();

    uint32 freeMemoryAtEnd = system_get_free_heap_size();

    if (freeMemoryAtEnd < freeMemoryAtStart)
    {
        Logger.error("Lost memory in " + String(name) + ": " + String(freeMemoryAtStart - freeMemoryAtEnd) + " bytes.");
    }
}

uint32 lastFreeMemory = 0;
uint32 freeMemory = 0;

void traceFreeMemory()
{
    freeMemory = system_get_free_heap_size();
    if (lastFreeMemory != freeMemory)
    {
        Logger.debug("Free Memory: " + String(freeMemory) + " bytes");
        lastFreeMemory = freeMemory;
    }

#ifdef DEBUG_MEMORY
    debugMemory();
#endif
}