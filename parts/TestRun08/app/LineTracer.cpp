/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

// 奥山追加 <begin>
// PIDクラスへ転換
const float LineTracer::INTERVAL = 0.004;  /* 制御間隔 4 [ms] */
const int   LineTracer::TURN_MAX = 100;    /* 操作量の最大値 */
const int   LineTracer::TURN_MIN = (-100); /* 操作量の最小値 */
const float LineTracer::KC = 1.20;            /* 限界感度法による持続振動時の比例ゲイン */
const float LineTracer::TC = 0.792;        /* 限界感度法による持続振動の周期 */
const float LineTracer::KP = (0.6 * KC);   /* 比例動作の比例係数 */
const float LineTracer::TI = (0.5 * TC);   /* 積分動作の比例係数 */
const float LineTracer::TD = (0.125 * TC); /* 微分動作の比例係数 */

//  灰色マーカ検知のしきい値
const float  LineTracer::GRAY_BAND_MIN = 36.0;
const float  LineTracer::GRAY_BAND_MAX = 46.0;
// 奥山追加 <end>

/**
 * コンストラクタ
 * @param Navigator     ライン判定
 * @param balancingWalker 倒立走行
 */
LineTracer::LineTracer(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator, balancingWalker, tail) {
        mLogging = new Logging();
        callCount = 0;
}

/**
 * ライントレースする
 */
int LineTracer::run() {

    int speed = 50;

    // 初期化処理 (最初1回だけ)
    if (mIsInitialized == false) {
        //mBalancingWalker->init();
        mIsInitialized = true;
        mAvgCnt=0;
    }
    
    // 尻尾を固定
    tail_control(TAIL_ANGLE_DRIVE);

    // 現在の明るさを求める
    int   brightness    = mNavigator->getBrightness();
    // 平均の明るさを求める
    float avgBrightness = mNavigator->getAverageBrightness();

    // 灰色検出を試みる
    if( avgBrightness >= GRAY_BAND_MIN && avgBrightness <= GRAY_BAND_MAX &&
        (mNavigator->getMaxBrightness() - mNavigator->getMinBrightness()) < 18 ){
        //mLogging->send(avgBrightness);

        // 直進中なら、灰色マーカの可能性大
        //if ( -1.0 <= mCurDirection && mCurDirection <= 1.0 )
        //{
            mAvgCnt++;
            // 灰色マーカかも
            if( mAvgCnt >= 16 ){
                if (mSubState == 0 ) ev3_speaker_play_tone(NOTE_D4, 200);
                mSubState = 1; // 灰色マーカモード
                speed = 30;    // スピードを抑えめに
                // 閾値 明るめに変えるよ
                mGrayThreshold = (int)avgBrightness+6;
                mNavigator->setThreshold(mGrayThreshold);
            }
        //}
        //else
        //{
        //    ;
        //}
    }
    // 黒か白だよ
    else {
        mSubState = 0;
        speed = 50;
        mNavigator->setThreshold(Navigator::INITIAL_THRESHOLD);
        mAvgCnt = 0;
    }

    // 走行体の向きを計算する
    int direction = calcDirection(brightness);
    //int direction = calcDirection(avgBrightness);

    mBalancingWalker->setCommand(speed, direction);

    // 倒立走行を行う
    mBalancingWalker->run();

    return 0;
}


// 奥山追加 <begin>
int LineTracer::calcDirection(int brightness){
    int turn;
    /* PID 制御用　操作量の算出 */
    /* 観測値を取得 */
    e_t = (float)(mNavigator->getThreshold()- brightness); /* 偏差を算出 */

    // PID制御
    int_e_t = int_e_t + e_t * INTERVAL; /* 積分項を算出 */
    der_e_t = (e_t - prev_e_t) / INTERVAL; /* 微分項を算出 */
    u_t = KP * (e_t + int_e_t / TI + TD * der_e_t); /* 操作量を計算 */

    // 灰色なら
    if ( mSubState == 1){
        if( u_t < 0 ){
            u_t = u_t * 0.1; // ハンドル操作を抑制
        }
    }
    
    // ハンドル抑制
    u_t = u_t * 0.9;

    temp_turn = -(int)u_t; /* 操作量を整数化 */

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
