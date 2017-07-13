/******************************************************************************
 *  BalancingWalker.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Class BalancingWalker
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "BalancingWalker.h"

// 定数宣言
const int BalancingWalker::LOW    = 30;    // 低速
const int BalancingWalker::NORMAL = 50;    // 通常
const int BalancingWalker::HIGH   = 70;    // 高速

/**
 * コンストラクタ
 * @param gyroSensor ジャイロセンサ
 * @param leftWheel  左モータ
 * @param rightWheel 右モータ
 * @param balancer   バランサ
 */
BalancingWalker::BalancingWalker(const ev3api::GyroSensor& gyroSensor,
                                 ev3api::Motor& leftWheel,
                                 ev3api::Motor& rightWheel,
                                 Balancer* balancer)
    : mGyroSensor(gyroSensor),
      mLeftWheel(leftWheel),
      mRightWheel(rightWheel),
      mBalancer(balancer),
      mForward(LOW),
      mTurn(LOW),
      mMode(0) {
}

/**
 * バランス走行する
 */
void BalancingWalker::run() {
    int16_t angle = mGyroSensor.getAnglerVelocity();  // ジャイロセンサ値
    int rightWheelEnc = mRightWheel.getCount();       // 右モータ回転角度
    int leftWheelEnc  = mLeftWheel.getCount();        // 左モータ回転角度

    mBalancer->setCommand(mForward, mTurn);

    int battery = ev3_battery_voltage_mV();
    mBalancer->update(angle, rightWheelEnc, leftWheelEnc, battery);

    // 左右モータに回転を指示する
    mLeftWheel.setPWM(mBalancer->getPwmLeft());
    mRightWheel.setPWM(mBalancer->getPwmRight());
}


/**
 * バランス走行に必要なものをリセットする
 */
void BalancingWalker::init() {
    int offset = mGyroSensor.getAnglerVelocity();  // ジャイロセンサ値

    // モータエンコーダをリセットする
    mLeftWheel.reset();
    mRightWheel.reset();

    // 倒立振子制御初期化
    mBalancer->init(offset);
}

/**
 * PWM値を設定する
 * @param forward 前進値
 * @param turn    旋回値
 */
void BalancingWalker::setCommand(int forward, int turn) {
    mForward = forward;
    mTurn    = turn;
}


int BalancingWalker::brake(int brake){
    // int lPwm, rPwm;
    // lPwm = mLeftWheel.getPWM();
    // rPwm = mRightWheel.getPWM();

    mForward -= brake;

    return mForward;
}

void BalancingWalker::blindWalk(int lPwmMotor, int rPwmMotor){
    // 左右モータに回転を指示する
    mLeftWheel.setPWM(  lPwmMotor );
    mRightWheel.setPWM( rPwmMotor );
}

void BalancingWalker::setSpinMode()
{
    mMode = 99;
    mRWheelOffset = mRightWheel.getCount();// 右モータ回転角度
    mLWheelOffset = mRightWheel.getCount();// 左モータ回転角度
    mRWheelCount = 0;
    mLWheelCount = 0;
}
void BalancingWalker::unsetSpinMode()
{
    mMode = 0;
}
int BalancingWalker::spin( int round )
{
    mLWheelCount = mLeftWheel.getCount() - mLWheelOffset;
    mRWheelCount = mRightWheel.getCount() - mRWheelOffset;
    
    // Modeチェック
    if ( mMode != 99 ) return -1;

    if ( mRWheelCount >= round || mLWheelCount <= -(round))
    {
        return 1; // End of Turn
    }
    mLeftWheel.setPWM(-10);
    mRightWheel.setPWM(10);
    return 0; // Must Continue (Not Ent of Turn)
}