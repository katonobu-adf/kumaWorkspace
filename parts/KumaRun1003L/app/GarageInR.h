/******************************************************************************
 *  GarageInR.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/09/21
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef GARAGEIN_R_H_
#define GARAGEIN_R_H_

#include "LookUpGate.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "PID.h"
#include "Logging.h"

class GarageInR : public LineTracer {
public:
    GarageInR(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);

public:
    int run();

    void setActiveMode(int activeMode);
    int getActiveMode();

    void setDistanceToGarage(int distanceToGarage);
    int getDistanceToGarage();

private:
    typedef struct {
        int value;
        int time;
        int distance;
    } GrayScaleOnGround;

private:
    void Distance_init();
    void Distance_update();
    float Distance_getDistance();
    float Distance_getDistance4msRight();
    float Distance_getDistance4msLeft();
    int judgeEnvironment(GrayScaleOnGround max, GrayScaleOnGround min);

private:
    
    int cyc_cnt; 
    int active_mode; 
    
    bool mIsInitialized;

    int forward;

    float distance;     //走行距離
    float distance4msL; //左タイヤの4ms間の距離
    float distance4msR; //右タイヤの4ms間の距離
    float distanceL; //左タイヤの距離
    float distanceR; //右タイヤの距離
    float pre_angleL;
    float pre_angleR; // 左右モータ回転角度の過去値
    int leftWheelOffset;
    int rightWheelOffset;

    GrayScaleOnGround onGroundMax;
    GrayScaleOnGround onGroundMin;

    int groundEnvironment;
    int turnForRecoveryTime;
    int distanceToGarage;

    bool notWhiteAreaDetected;
    bool notYetDetectBlack;
    
    // ログ取得用
    Logging* mLogging;
};

#endif  // GARAGEIN_R_H_
