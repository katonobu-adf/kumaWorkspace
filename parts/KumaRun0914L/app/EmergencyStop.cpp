
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
    : Task(navigator, balancingWalker, tail)
{
    ;
}

// Override
int EmergencyStop::run(){
    stop();
    return 1;
}

int EmergencyStop::stop(){
    
    // Test
    int32_t rw, lw;
    char buf[64];
    lw = mNavigator->getLWheelCount();
    rw = mNavigator->getRWheelCount();
    sprintf(buf, "L:%5ld, R:%5ld", lw, rw );
    ev3_lcd_draw_string( buf, 0, 90);
    // Test

    mBalancingWalker->stop();
    tail_control( TAIL_ANGLE_DRIVE, 10 );
    return 1;
}
