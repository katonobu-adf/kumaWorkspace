/******************************************************************************
 *  GarageIn.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef GARAGEIN_H_
#define GARAGEIN_H_

#include "LineTracer.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "Logging.h"

class GarageIn : public LineTracer {
public:
    GarageIn(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();
    
private:
    int speedControl(int direction);
    int tail_control(signed int p_gain);

private:

private:
    // ログ取得用
    // Logging* mLogging;

    int speedControlExecFlagCount;
    bool mIsInitialized;
    bool mStopped;
    bool mInStoppedAtFirst;
    bool mLastMoving;
    int32_t distance;
    int forward;
    int turn;
    int targetDistance;
    int tailAngleStandUp;
    int tailAngleDrive;
    int pulse;
    int runningForLandingCount;
    int tailStatus;
    float p_gain;
    bool arrived;

    int leftWheelOffset;
    int rightWheelOffset;

};

#endif  // GARAGEIN_H_
