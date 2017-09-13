
/******************************************************************************
 *  Driver.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef  DRIVER_H
#define  DRIVER_H

#include "ev3api.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "Task.h"
#include "Calibration.h"
#include "ReadyToStart.h"
#include "StartToDash.h"
#include "LineTracer.h"
#include "EmergencyStop.h"
// ADF)加藤
#include "SpeedDown.h"
#include "LookUpGate.h"
// ADF)加藤

class Driver {

public:
    static const int  EMERGENCY_STOP;   // 緊急停止

    static const int  CALIBRATION;      // キャリブレーション
    static const int  READY_TO_START;   // スタート前起立 (Start合図待ち)
    static const int  START_TO_DASH;    // (安定化) スタート
    static const int  LINE_TRACER;      // ライントレース
    static const int  WALK_ON_GRAY;     // 灰色マーカ上走行
    // ADF)加藤
    static const int  SPEED_DOWN;       // 難所前のつなぎ
    static const int  LOOK_UP_GATE;     // ルックアップゲート
    // ADF)加藤

public:

    Driver( Navigator * navigator,
            BalancingWalker* balancingWalker,
            ev3api::Motor &tail);
    int run(void);

    // ADF)加藤
    int getState();
    // ADF)加藤

private:
    Navigator * mNavigator;
    BalancingWalker* mBalancingWalker;
    ev3api::Motor& mTail;
    bool mIsInitialized;

private:
    Task  * mTask[10];
    EmergencyStop * emergencyStop;
    // ADF)加藤
    SpeedDown *speedDown;
    // ADF)加藤

};

#endif  /* DRIVER_H */
