#include "EmergencyStop.h"

EmergencyStop::EmergencyStop(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator, balancingWalker, tail)
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
    tail_control( TAIL_ANGLE_DRIVE, 10 );
    return 1;
}
