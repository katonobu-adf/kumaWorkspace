
/******************************************************************************
 *  ReadyToStart.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   READYTOSTART_H
#define   READYTOSTART_H

#include "TaskHolder.h"

// extends TaskHolder
class ReadyToStart : public TaskHolder {
public:
    ReadyToStart(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
};

#endif    /* READYTOSTART_H */