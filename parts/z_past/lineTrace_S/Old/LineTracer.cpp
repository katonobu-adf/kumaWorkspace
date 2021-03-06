/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

// 奥山追加 <begin>

const float LineTracer::INTERVAL = 0.004;  /* 制御間隔 4 [ms] */
const int   LineTracer::TURN_MAX = 100;    /* 操作量の最大値 */
const int   LineTracer::TURN_MIN = (-100); /* 操作量の最小値 */
const float LineTracer::KC = 3;            /* 限界感度法による持続振動時の比例ゲイン */
const float LineTracer::TC = 0.484;        /* 限界感度法による持続振動の周期 */
const float LineTracer::KP = (0.6 * KC);   /* 比例動作の比例係数 */
const float LineTracer::TI = (0.5 * TC);   /* 積分動作の比例係数 */
const float LineTracer::TD = (0.125 * TC); /* 微分動作の比例係数 */

// 奥山追加 <end>
/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param balancingWalker 倒立走行
 */
LineTracer::LineTracer(const LineMonitor* lineMonitor,
                       BalancingWalker* balancingWalker)
    : mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mIsInitialized(false) {
}

/**
 * ライントレースする
 */
void LineTracer::run() {
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }

    //bool isOnLine = mLineMonitor->isOnLine();
    int  brightness = mLineMonitor->getBrightness();

    // 走行体の向きを計算する
    //int direction = calcDirection(isOnLine);
    int direction = calcDirection(brightness);

    mBalancingWalker->setCommand(BalancingWalker::LOW, direction);

    // 倒立走行を行う
    mBalancingWalker->run();
}

/**
 * 走行体の向きを計算する
 * @param isOnLine true:ライン上/false:ライン外
 * @retval 30  ライン上にある場合(右旋回指示)
 * @retval -30 ライン外にある場合(左旋回指示)
 */
int LineTracer::calcDirection(bool isOnLine) {
    if (isOnLine) {
        // ライン上にある場合
        return BalancingWalker::LOW;
    } else {
        // ライン外にある場合
        return -BalancingWalker::LOW;
    }
}

// 奥山追加 <begin>
int LineTracer::calcDirection(int brightness){
    int turn;
    /* PID 制御用　操作量の算出 */
    /* 観測値を取得 */
    e_t = (float)(mLineMonitor->getThreshold()- brightness); /* 偏差を算出 */

    int_e_t = int_e_t + e_t * INTERVAL; /* 積分項を算出 */
    der_e_t = (e_t - prev_e_t) / INTERVAL; /* 微分項を算出 */
    u_t = KP * (e_t + int_e_t / TI + TD * der_e_t); /* 操作量を計算 */
    temp_turn = (int)u_t; /* 操作量を整数化 */

    if (temp_turn > TURN_MAX) /* 操作量の上限設定 */
        temp_turn = TURN_MAX;
    if (temp_turn < TURN_MIN) /* 操作量の加減設定 */
        temp_turn = TURN_MIN;
    
    turn = (signed char)temp_turn; /* 操作量を確定 */

    /* 現在の偏差を次回の 1 ステップ前の偏差とする */
    prev_e_t = e_t;
    
    return turn;
}
// 奥山追加 <end>

