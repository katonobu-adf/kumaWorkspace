
/******************************************************************************
 *  SpeedDown.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/09/05
 *  Author: Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef SPEEDDOWN_H_
#define SPEEDDOWN_H_

#include "LineTracer.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "Logging.h"

class SpeedDown : public LineTracer {
public:
    SpeedDown(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();

    void setParameter(int currForward, int targetForward, int transitionTime);

private:

public:

private:
    int currForward;
    int targetForward;
    int transitionTime;
    int totalForwardDiff;
    int forwardDiff;
    bool underOnePer4mSec;
    int countForThroughing;
    int countForUnderOnePer4mSec;
    int speedDownValuePer4mSec;
};

#endif  // SPEEDDOWN_H_
