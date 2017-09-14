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
const float LineTracer::INTERVAL = 0.004;
const int   LineTracer::TURN_MAX = 100;
const int   LineTracer::TURN_MIN = (-100);

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
        LineTracer::pid = new PID*[5];
        pid[0] = new PID(2.80, 0.444);
        pid[1] = new PID(1.50, 0.588);
        pid[2] = new PID(2.00, 0.660);
        pid[3] = new PID(1.70, 0.732);
        pid[4] = new PID(1.20, 0.792);
        mLogging = new Logging();
        callCount = 0;
        temp_turn = 0;
}

/**
 * ライントレースする
 */
int LineTracer::run() {
    int forward;

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
    //float avgBrightness = mNavigator->getAverageBrightness();

    forward = calcForward(temp_turn);

    // 灰色検出を試みる
/*
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
                forward = 30;    // スピードを抑えめに
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
        forward = 50;
        mNavigator->setThreshold(Navigator::INITIAL_THRESHOLD);
        mAvgCnt = 0;
    }
*/
    // 走行体の向きを計算する
    int direction = calcDirection(brightness, forward);
    //int direction = calcDirection(avgBrightness, forward);

    mBalancingWalker->setCommand(forward,direction);

    // 倒立走行を行う
    mBalancingWalker->run();

    return 0;
}

int LineTracer::calcForward(int turn) {
    //return (100 - (abs(turn) / 25 * 10)) / 2; // 60 - 100
    return 50;
}// end calcForward


// <begin>
int LineTracer::calcDirection(int brightness, int forward){
    int turn;
    int key = (forward - 60) / 10;

    // Safe Code
    if ( key < 0 || key > 4 ) key = 0;

    e_t = (float)(mNavigator->getThreshold()- brightness);

    int_e_t = int_e_t + e_t * INTERVAL;
    der_e_t = (e_t - prev_e_t) / INTERVAL;
    u_t = pid[key]->getKP() * (e_t + int_e_t / pid[key]->getTI() + pid[key]->getTD() * der_e_t);

    // 灰色なら
    // if ( mSubState == 1){
    //     if( u_t < 0 ){
    //         u_t = u_t * 0.1; // ハンドル操作を抑制
    //     }
    // }
    
    // ハンドル抑制
    // u_t = u_t * 0.9;

    temp_turn = -(int)u_t;
    if (temp_turn > TURN_MAX)
        temp_turn = TURN_MAX;
    if (temp_turn < TURN_MIN)
        temp_turn = TURN_MIN;
    
    turn = (signed char)temp_turn;

    prev_e_t = e_t;

    //mLogging->send(turn);
    
    return turn;
}

int LineTracer::abs(int num) {
    if (num < 0)
        num *= -1;

    return num;
}// end abs
