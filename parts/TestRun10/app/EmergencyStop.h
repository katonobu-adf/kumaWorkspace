
/******************************************************************************
 *  EmergencyStop.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   EMERGENCY_STOP_H
#define   EMERGENCY_STOP_H

#include "Task.h"

// extends Task
class EmergencyStop : public Task {
public:
    EmergencyStop(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
    int stop();
};


#endif    /* EMERGENCY_STOP_H */