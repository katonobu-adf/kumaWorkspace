
/******************************************************************************
 *  LineTracer.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "LineTracer.h"

// PID制御用 定数
const float LineTracer::INTERVAL = 0.004;
const int   LineTracer::TURN_MAX = 100;
const int   LineTracer::TURN_MIN = (-100);

const int LineTracer::LOW      = 60;   // KC=2.80 TC=0.444
const int LineTracer::SECOND   = 70;   // KC=1.50 TC=0.588
const int LineTracer::THIRD    = 80;   // KC=2_00 TC=0.660
const int LineTracer::TOP      = 90;   // KC=1.70 TC=0.732
const int LineTracer::HIGHTOP  = 100;  // KC=1.20 TC=0.792

//  灰色マーカ検知のしきい値
const float  LineTracer::GRAY_BAND_MIN = 36.0;
const float  LineTracer::GRAY_BAND_MAX = 46.0;

// 走行距離による別タスク移行のしきい値
// L/Rコースのゴールまでの距離よりちょっと長いくらいを設定
const int LineTracer::RUNNING_DISTANCE_THRESHOLD = 15500; // 1100cm

const int PID_LENGTH = 5;

/**
 * コンストラクタ
 * @param navigator         Navigator
 * @param balancingWalker   倒立振子ライブラリのラッパ―
 * @param tail              尻尾のモーター
 */
LineTracer::LineTracer(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : Task(navigator, balancingWalker, tail) {
        // PID テーブルの初期化
        LineTracer::pid = new PID*[PID_LENGTH];
        pid[0] = new PID(3.00, 0.504);
        pid[1] = new PID(2.50, 0.572);
        pid[2] = new PID(2.00, 0.500);
        pid[3] = new PID(1.60, 0.656);
        pid[4] = new PID(1.30, 0.816);
        callCount = 0;
        temp_turn = 0;
        temp_forward = 0;
}

/**
 * ライントレースする
 */
int LineTracer::run() {
    int forward;

    // 初期化処理 (最初1回だけ)
    if (mIsInitialized == false) {
        mIsInitialized = true;
        mAvgCnt = 0;
    }

    // 尻尾を固定
    tail_control(TAIL_ANGLE_DRIVE);

    // 現在の明るさを求める
    int   brightness    = mNavigator->getBrightness();

    forward = calcForward(temp_turn, temp_forward);
    temp_forward = forward;

    // 走行体の向きを計算する
    int direction = calcDirection(brightness, forward);

    // 普通のライントレースを行う
    mBalancingWalker->setCommand(forward, direction);

    // 倒立走行を行う
    mBalancingWalker->run();

    // 走行距離が規定値以上になったら次のタスクへ移行する
    int runningDistance = (mNavigator->getLWheelCount() + mNavigator->getRWheelCount()) / 2;
    if (runningDistance >= RUNNING_DISTANCE_THRESHOLD) {
        return 1;
    }

    return 0;
}

int LineTracer::calcForward(int turn, int forward) {

    // forward の値を自動で決めて走行する
    int f;
    int ratio = abs(turn) + 5;
    ratio /= 10;

    switch (ratio) {
        case 0:// 0 ~ 4
            f = calcControler(forward, HIGHTOP);
            break;
        case 1:// 5 ~ 14
            f = calcControler(forward, TOP);
            break;
        case 2:// 15 ~ 24
            f = calcControler(forward, THIRD);
            break;
        case 3:// 25 ~ 34
            f = calcControler(forward, SECOND);
            break;
        default:// 35 ~ 100
            f = calcControler(forward, LOW);
            break;
    }// switch

    return f;
}// end calcForward

int LineTracer::calcControler(int forward, int gear) {
    if (forward == gear) {
        return gear;
    } else if (forward > gear) {
        if (forward - gear > 10)
            return forward - 10;
        else
            return gear;
    } else {
        if (gear - forward > 10)
            return forward + 10;
        else
            return gear;
    }// end if
}// end calcControler

// <begin>
int LineTracer::calcDirection(int brightness, int forward){
    int turn;
    int key = (forward - 60) / 10;

    // Safe Code
    if ( key < 0 || key > PID_LENGTH - 1 ) key = 0;

    e_t = (float)(mNavigator->getThreshold()- brightness);
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

    return turn;
}

int LineTracer::abs(int num) {
    if (num < 0)
        num *= -1;

    return num;
}// end abs
