#include "EmergencyStop.h"

EmergencyStop::EmergencyStop(
           Navigator * navigator,
           LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail)
{
    ;
}

// Override
int EmergencyStop::run(){
    stop();
    return 1;
}

int EmergencyStop::stop(){
    mBalancingWalker->stop();
    return 1;
}
