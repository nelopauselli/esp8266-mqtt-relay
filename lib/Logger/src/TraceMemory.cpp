#include "Arduino.h"
#include "Logger.h"

//#define DEBUG_MEMORY

extern "C" {
#include "user_interface.h"
}

void debugMemory()
{
    DEBUG("[MEMORY] ");
    DEBUGLN(system_get_free_heap_size());
}

void traceMemoryLeak(const char *name, void (*callback)())
{
    uint32 freeMemoryAtStart = system_get_free_heap_size();

    callback();

    uint32 freeMemoryAtEnd = system_get_free_heap_size();

    if (freeMemoryAtEnd < freeMemoryAtStart)
    {
        DEBUG("[ERROR] Lost memory in ");
        DEBUG(name);
        DEBUG(": ");
        DEBUG(freeMemoryAtStart - freeMemoryAtEnd);
        DEBUGLN(" bytes.");
    }
}

uint32 lastFreeMemory = 0;
uint32 freeMemory = 0;

void traceFreeMemory()
{
    freeMemory = system_get_free_heap_size();
    if (lastFreeMemory != freeMemory)
    {
        DEBUG("Free Memory: ");
        DEBUG(freeMemory);
        DEBUGLN(" bytes");
        
        lastFreeMemory = freeMemory;
    }

#ifdef DEBUG_MEMORY
    debugMemory();
#endif
}