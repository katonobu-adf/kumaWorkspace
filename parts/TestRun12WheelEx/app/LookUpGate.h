
/******************************************************************************
 *  LookupGate.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef LOOKUPGATE_H_
#define LOOKUPGATE_H_

#include "Task.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "PID.h"
#include "Logging.h"

class LookUpGate : public Task{
public:
    LookUpGate(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();

private:

    void Distance_init();
    void Distance_update();
    float Distance_getDistance();
    float Distance_getDistance4msRight();
    float Distance_getDistance4msLeft();

private:
    static const float DISTANCE_TO_GARAGE;
    
private:
    // ログ取得用
    Logging* mLogging;
};

#endif  // LOOKUPGATE_H_
