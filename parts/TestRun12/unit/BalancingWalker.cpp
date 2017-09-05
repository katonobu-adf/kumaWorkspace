/******************************************************************************
 *  BalancingWalker.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Class BalancingWalker
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "BalancingWalker.h"

// 定数宣言
const int BalancingWalker::LOW      = 60;   // KC=2.80 TC=0.444
const int BalancingWalker::SECOND   = 70;   // KC=1.50 TC=0.588
const int BalancingWalker::THIRD    = 80;   // KC=2_00 TC=0.660
const int BalancingWalker::TOP      = 90;   // KC=1.70 TC=0.732
const int BalancingWalker::HIGHTOP  = 100;  // KC=1.20 TC=0.792

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
        mLogging = new Logging();
        ;
}

/**
 * バランス走行に必要なものをリセットする
 */
void BalancingWalker::init() {
    int offset = mGyroSensor.getAnglerVelocity();  // ジャイロセンサ値

    // モータエンコーダをリセットする
    mRightWheel.reset();
    mLeftWheel.reset();

    // 倒立振子制御初期化
    mBalancer->init(offset);
    
    // 奥山
    mRWheelCount = mLWheelCount = 0;
    mVelocity = 0;
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

/**
 * バランス走行する
 */
void BalancingWalker::run() {
    int16_t angleVelocity = mGyroSensor.getAnglerVelocity();  // ジャイロセンサ値
    
    // ジャイロ値のロギング
    // mLogging->send(angleVelocity);

    mVelocity = angleVelocity;

    int rightWheelEnc = mRightWheel.getCount();       // 右モータ回転角度
    int leftWheelEnc  = mLeftWheel.getCount();        // 左モータ回転角度

    mBalancer->setCommand(mForward, mTurn);

    int battery = ev3_battery_voltage_mV();
    mBalancer->update(angleVelocity, rightWheelEnc, leftWheelEnc, battery);

    // 左右モータに回転を指示する
    int rPWM,lPWM;
    rPWM = mBalancer->getPwmRight();
    lPWM = mBalancer->getPwmLeft();
    mRightWheel.setPWM(rPWM);
    mLeftWheel.setPWM(lPWM);
    
    // mLogging->send(rPWM);
    // mLogging->send(lPWM);
    
    mRWheelOffset = mRightWheel.getCount();// 右モータ回転角度
    mLWheelOffset = mLeftWheel.getCount();// 左モータ回転角度
}

void BalancingWalker::stop(void){
    mRightWheel.setPWM(0);
    mLeftWheel.setPWM(0);
    return;
}

int BalancingWalker::brake(int brake)
{
    mForward -= brake;
    return mForward;
}

void BalancingWalker::blindWalk(int lPwmMotor, int rPwmMotor){
    // 左右モータに回転を指示する
    mLeftWheel.setPWM(  lPwmMotor );
    mRightWheel.setPWM( rPwmMotor );
}

int BalancingWalker::getRWheelOffset(void){
    return mRWheelOffset;
}

int BalancingWalker::getLWheelOffset(void){
    return mLWheelOffset;
}

int BalancingWalker::getSavedVelocity(void){
    return mVelocity;
}
