
#include "ReadyToStart.h"

ReadyToStart::ReadyToStart(
           Navigator * navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail)
{
    ;
}

// Override
int ReadyToStart::run(){

    int tailState=tail_control(TAIL_ANGLE_STAND_UP);
    
    if ( tailState == 1 ){
        if ( mNavigator->isTouchPressed() || 
             mNavigator->getBtCommand() == 1 ){
                 return 1;
        }
    }
    return 0;
}
