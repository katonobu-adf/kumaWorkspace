
/******************************************************************************
 *  Calibration.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   CALIBRATION_H
#define   CALIBRATION_H

#include "TaskHolder.h"
#include "Logging.h"

// extends TaskHolder
class Calibration : public TaskHolder {
public:
    Calibration(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
private:
    // ログ取得用
    Logging* mLogging;
    char buf[128];
};


#endif    /* CARIBRATION_H */