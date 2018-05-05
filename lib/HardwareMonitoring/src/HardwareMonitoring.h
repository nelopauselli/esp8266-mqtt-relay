#ifndef HARDWARE_MONITORING_CLASS
#define HARDWARE_MONITORING_CLASS

#include "Subject.h"
#include "HardwareMonitoringArgs.h"

class HardwareMonitoring : public Subject<HardwareMonitoringArgs>
{
  public:
    void process();
};

#endif
