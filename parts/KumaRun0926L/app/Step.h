/******************************************************************************
 *  Step.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Rena Onishi, Yukou Sugawara, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef STEP_H_
#define STEP_H_

#include "LineTracer.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "PID.h"
#include "Logging.h"

class Step : public LineTracer {
public:
    Step(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);

public:
    int run();

private:
    void Distance_init();
    void Distance_update();
    float Distance_getDistance();
    float Distance_getDistance4msRight();
    float Distance_getDistance4msLeft();
    void setDistanceToGarage();

private:
    float distanceToGarage;
    
    int cyc_cnt; 
    int active_mode; 
    
    bool mIsInitialized;

    int forward;

    float distance;     //走行距離
    float distance4msL; //左タイヤの4ms間の距離
    float distance4msR; //右タイヤの4ms間の距離
    float pre_angleL;
    float pre_angleR; // 左右モータ回転角度の過去値
    int leftWheelOffset;
    int rightWheelOffset;

    int defaultThreshold;
    int currThreshold;

    bool edgeChanged;

    int stepCount;

    int noResponseTimeRemain;

    bool in1stStep;
    bool in2ndStep;
    bool in2ndStepMiddle;
    float notes[3];
    
    // ログ取得用
    Logging* mLogging;
};

#endif  // STEP_H_
