#include "HardwareMonitoring.h"
#include "HardwareMonitoringArgs.h"

extern "C" {
#include "user_interface.h"
}

void HardwareMonitoring::process()
{
    HardwareMonitoringArgs args;
    args.freeMemory = system_get_free_heap_size();

    Subject::notify(args);
}