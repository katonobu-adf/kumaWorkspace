
/******************************************************************************
 *  SpeedDown.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/09/05
 *  Author: Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "SpeedDown.h"

// using宣言
using ev3api::Motor;

// オブジェクトの定義

/**
 * コンストラクタ
 * @param mNavigator         mNavigator
 * @param balancingWalker   倒立振子ライブラリのラッパ―
 * @param tail              尻尾のモーター
 */
SpeedDown::SpeedDown(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : LineTracer (navigator, balancingWalker, tail) {

        currForward = 0;
        targetForward = 0;
        transitionTime = 0;
        totalForwardDiff = 0;
        forwardDiff = 0;
        underOnePer4mSec = false;
        countForThroughing = 0;
        countForUnderOnePer4mSec = 0;
}


/**
 * スピードダウン
 */
int SpeedDown::run() {
    // 走行速度を設定する
    if (underOnePer4mSec) {
        if((countForUnderOnePer4mSec % countForThroughing) == 0) {
            currForward -= forwardDiff;
        }
        ++countForUnderOnePer4mSec;
    } else {
        currForward -= forwardDiff;
    }
    // 現在の明るさを求める
    int   brightness    = mNavigator->getBrightness();
    // 走行体の向きを計算する
    int direction = calcDirection(brightness, currForward);

    mBalancingWalker->setCommand(currForward , direction);
    mBalancingWalker->run();
    if (currForward <= targetForward) {
        return 1;
    }
    return 0;
}


void SpeedDown::setParameter(int currForward, int targetForward, int transitionTime) {
    ev3_speaker_play_tone(NOTE_C4, 200);
    
    this->currForward = currForward;
    this->targetForward = targetForward;
    this->transitionTime = transitionTime;

    // 計算(マイナスは無い前提->これにも対応できるならSpeedDownじゃなくてSpeedChangeにする)
    totalForwardDiff = currForward - targetForward;

    // 指定時間(msec単位)での目標速度へ減速するの場合、4msec毎にどれくらい減速すれば良いか計算
    double speedDownValuePer4mSec = ((double)totalForwardDiff / (double)transitionTime) * 4.0;

    if (speedDownValuePer4mSec < 1.0) {
        // 4msec毎の減速値が1より小さい場合は減速値を1とし、
        // 4msecの倍数から減速値が1に近いカウントを求める
        // 4msec毎のロジックをカウント分毎スルー（4msec * カウント秒毎に発動）し、都度1つ減速を行う
        underOnePer4mSec = true;
        forwardDiff = 1;
        countForThroughing = 1 / speedDownValuePer4mSec;
    } else {
        // 4msec毎減速値が1より大きければそのまま採用
        forwardDiff = (int)speedDownValuePer4mSec;
    }
}
