
/******************************************************************************
 *  StartToDash.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   START_TO_DASH_H
#define   START_TO_DASH_H

#include "Task.h"
#include "Logging.h"

// extends Task
class StartToDash : public Task {
public:
    StartToDash(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
private:
    static const int FIRST_STEP;
    static const int SECOND_STEP;
    static const int THIRD_STEP;
    static const int FOURTH_STEP;
    static const int FIFTH_STEP;
    
    int callCount;
    int speed;

private:
    // ログ取得用
    Logging* mLogging;
};

#endif    /* START_TO_DASH_H */