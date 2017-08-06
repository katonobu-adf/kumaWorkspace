/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

// <begin>

const float LineTracer::INTERVAL = 0.004;
const int   LineTracer::TURN_MAX = 100;
const int   LineTracer::TURN_MIN = (-100);

// <end>

/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param balancingWalker 倒立走行
 */
LineTracer::LineTracer(
           Navigator* navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker, tail) {
        LineTracer::pid = new PID*[5];
        pid[0] = new PID(2.80, 0.444);
        pid[1] = new PID(1.50, 0.588);
        pid[2] = new PID(2.00, 0.660);
        pid[3] = new PID(1.70, 0.732);
        pid[4] = new PID(1.20, 0.792);
        mLogging = new Logging();
        callCount = 0;
          ;
}

/**
 * ライントレースする
 */
int LineTracer::run() {
    // ‰œŽR
    // 10•bƒ‰ƒCƒ“ƒgƒŒ[ƒX‚µ‚½‚ç•Ê‚Ì‚±‚Æ‚â‚ê
    // callCount++;
    // if (callCount > 1000) {
    //     return 1;
    // }

    // ‰œŽR
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }
    
    tail_control(TAIL_ANGLE_DRIVE);
    int  brightness = mLineMonitor->getBrightness();

    // 速度を計算する
    int forward = calcForward(temp_turn);

    // 走行体の向きを計算する
    int direction = calcDirection(brightness, forward);

    mBalancingWalker->setCommand(forward, direction);

    // 倒立走行を行う
    mBalancingWalker->run();

    return 0;
}

int LineTracer::calcForward(int turn) {
    return (100 - abs(turn) / 25 * 10);// 60 ～ 100　の五段階
}// end calcForward

// <begin>
int LineTracer::calcDirection(int brightness, int forward) {
    int turn;
    int key = (forward - 60) / 10;

    e_t = (float)(mLineMonitor->getThreshold()- brightness);

    int_e_t = int_e_t + e_t * INTERVAL;
    der_e_t = (e_t - prev_e_t) / INTERVAL;
    u_t = pid[key]->getKP() * (e_t + int_e_t / pid[key]->getTI() + pid[key]->getTD() * der_e_t);
    temp_turn = -(int)u_t;

    if (temp_turn > TURN_MAX)
        temp_turn = TURN_MAX;
    if (temp_turn < TURN_MIN)
        temp_turn = TURN_MIN;
    
    turn = (signed char)temp_turn;

    prev_e_t = e_t;

    mLogging->send(turn);

    return turn;
}

int LineTracer::abs(int num) {
    if (num < 0)
        num *= -1;

    return num;
}// end abs