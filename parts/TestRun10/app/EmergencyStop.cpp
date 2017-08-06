
/******************************************************************************
 *  EmergencyStop.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

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
