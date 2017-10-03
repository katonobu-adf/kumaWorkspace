
/******************************************************************************
 *  ReadyToStart.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   READYTOSTART_H
#define   READYTOSTART_H

#include "Task.h"

// extends Task
class ReadyToStart : public Task {
public:
    ReadyToStart(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();

private:
    int tail_adjust = 0;    // 尻尾の傾きの微調整用

    int interval = 0;       // クールダウンのカウント

    // ディスプレイ表示用
    char buf[128];
};

#endif    /* READYTOSTART_H */