/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "UpStep.h"

// 奥山追加 <begin>

// const float LineTracer::INTERVAL = 0.004;  /* 制御間隔 4 [ms] */
// const int   LineTracer::TURN_MAX = 100;    /* 操作量の最大値 */
// const int   LineTracer::TURN_MIN = (-100); /* 操作量の最小値 */
// const float LineTracer::KC = 3;            /* 限界感度法による持続振動時の比例ゲイン */
// const float LineTracer::TC = 0.484;        /* 限界感度法による持続振動の周期 */
// const float LineTracer::KP = (0.6 * KC);   /* 比例動作の比例係数 */
// const float LineTracer::TI = (0.5 * TC);   /* 積分動作の比例係数 */
// const float LineTracer::TD = (0.125 * TC); /* 微分動作の比例係数 */

// 奥山追加 <end>
/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param balancingWalker 倒立走行
 */
UpStep::UpStep(
           Navigator * navigator,
           const LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail) {
        callCount=0;
          ;
}

/**
 * ライントレースする
 */
int UpStep::run() {
    // 奥山
    // 10秒ライントレースしたら別のことやれ
    callCount++;
    if (callCount > 2500) {
        return 1;
    }

    // 奥山
    
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }
    
    tail_control(TAIL_ANGLE_DRIVE);
    //int  brightness = mLineMonitor->getBrightness();

    // 走行体の向きを計算する
    //int direction = calcDirection(brightness);

    mBalancingWalker->setCommand(BalancingWalker::LOW, 0);

    // 倒立走行を行う
    mBalancingWalker->run();

    return 0;
}


// 奥山追加 <begin>

// 奥山追加 <end>

