/******************************************************************************
 *  GarageIn.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "GarageIn.h"

#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */
#define P_GAIN             2.0F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */

// using宣言
using ev3api::Motor;
using ev3api::TouchSensor;

// オブジェクトの定義

/**
 * コンストラクタ
 * @param mNavigator         mNavigator
 * @param balancingWalker   倒立振子ライブラリのラッパ―
 * @param tail              尻尾のモーター
 */
GarageIn::GarageIn(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : LineTracer (navigator, balancingWalker, tail) {

    // mLogging = new Logging();
    
    speedControlExecFlagCount = 0;
    mIsInitialized = false;
    mStopped = false;
    mInStoppedAtFirst = true;
    mLastMoving = true;
    distance = 0;
    forward = 30;
    turn = 0;
    // 1m設定 forward:30で11cm程度オーバー
    // 4はm/rotate=1/4（つまり4回転で約1m）から
    // targetDistance = ((float)100 / 100.0) * 4 * 360;;
    targetDistance = ((float)86.0 / 100.0) * 4 * 360;;
    tailAngleStandUp = 70;
    tailAngleDrive = 3;
    pulse = -10;
    runningForLandingCount = 0;
    tailStatus = 0;
    p_gain =  2.0F;
    arrived = false;
    leftWheelOffset = 0;
    rightWheelOffset = 0;

}


/**
 * ガレージイン
 */
int GarageIn::run() {

    // しっぽおろす
    // tail_control(80);

    // 現在の明るさを求める
    int   brightness    = mNavigator->getBrightness();
    // 走行体の向きを計算する
    int direction = calcDirection(brightness, forward);

    // ライントレースさせたくない場合は0にする
    direction = 0;

    // mLogging->send(direction);

    if(mStopped == false && (++speedControlExecFlagCount % 62) == 0 )
    {
        // 約250msecごとに実行
        // 別サイクリックタスクにしていたので無理やり移植
        speedControl(direction);
    }

    // 倒立走行
    if (mIsInitialized == false) {
        mIsInitialized = true;
        // mBalancingWalker->init();
        mBalancingWalker->setCommand(forward, direction);

        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
    }
    
    // 倒立走行を行う  
    if (mStopped)
    {
        if (mInStoppedAtFirst)
        {
            mBalancingWalker->setCommand(forward, direction);
            tailStatus = 0;
            mInStoppedAtFirst = false;
            mBalancingWalker->run();
        }

        // しっぽおろす
        if (tailStatus == 0)
        {
            // p_gain = 0.6f;
            // gBalancingWalker->setCommand(40, 0);
            tailStatus = tail_control(tailAngleStandUp); /* 完全停止用角度に制御 */
            mBalancingWalker->run();
        }
        else
        {
            // 少し後進し、後ろ向きの慣性を作って止める
            // if (tailStatus == 1 && runningForLandingCount < 250)
            if (tailStatus == 1 && runningForLandingCount < 375)
            {
                forward = pulse;
                runningForLandingCount++;
                mBalancingWalker->setCommand(forward, 0);
                mBalancingWalker->run();
            } 
            else 
            {
                if (mLastMoving && mBalancingWalker->getSavedVelocity() < -10)
                {
                    forward = 0;
                    mLastMoving = false;
                    mBalancingWalker->setCommand(forward, 0);
                    mBalancingWalker->stop();
                    // return 1;
                }
                else if(mLastMoving)
                {
                    mBalancingWalker->run();
                }
            }
        }

    } 
    else {
        if (tailStatus == 0)
        {
            tailStatus = tail_control(tailAngleDrive);
        }
        
        // if (arrived) {
        //     // 指定距離到達なら直進のみ
        //     direction = 0;
        // }
        mBalancingWalker->setCommand(forward, direction);
        mBalancingWalker->run();
    }
    return 0;
}

/**
 * スピード制御タスク
 */
int GarageIn::speedControl(int direction) {

    // static bool arrived = false;

    // 8.2cm * PI = 25.748cm
    // 8回転で2m 8 * 360 = 2880
    int leftDistance = mNavigator->getLWheelCount() - leftWheelOffset;
    int rightDistance =  mNavigator->getRWheelCount() - rightWheelOffset;
    distance = (leftDistance + rightDistance) / 2;
    
    if (distance > targetDistance && mStopped == false) {
        if(arrived == false) 
        {
            ev3_speaker_play_tone(NOTE_C5, 200);
            // tslp_tsk(200);
            // ev3_speaker_play_tone(NOTE_C4, 200);
            arrived = true;
        }        
        // 減速モード
        // 1だとなかなか止まらない
        // 5だと50cmくらいオーバーラン＆停止に8秒かかる
        // 10は？とりあえず転倒はしない
        forward -= 10;
        if(forward < 5) {
            forward = 0;
        }

        if (forward == 0) {
            mStopped = true;
            // mBalancingWalker->setCommand(forward, 0);
        } 
        else
        {
            mBalancingWalker->setCommand(forward, direction);
        }
    }

    return 0;
}


int GarageIn::tail_control(signed int angle)
{
    // float pwm = (float)(angle - mTail.getCount())*P_GAIN; /* 比例制御 */
    float pwm = (float)(angle - mTail.getCount())*p_gain; /* 比例制御 */
    /* PWM出力飽和処理 */
    if (pwm > PWM_ABS_MAX)
    {
        pwm = PWM_ABS_MAX;
    }
    else if (pwm < -PWM_ABS_MAX)
    {
        pwm = -PWM_ABS_MAX;
    }

    if (pwm == 0)
    {
        mTail.setPWM(pwm);
        mTail.stop();
        // ev3_motor_stop(EV3_PORT_A, true);
        return 1;
    }
    else
    {
        mTail.setPWM(pwm);
        // ev3_motor_set_power(EV3_PORT_A, (signed char)pwm);
    }
    return 0;
}

