
/******************************************************************************
 *  Task.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Task.h"

const float Task::P_GAIN=2.5F;                    /* 完全停止用モータ制御比例係数 */
const int   Task::PWM_ABS_MAX = 60;               /* 完全停止用モータ制御PWM絶対最大値 */
const int   Task::TAIL_ANGLE_STAND_UP      = 92;  /* 完全停止時の角度[度] */
const int   Task::TAIL_ANGLE_START_TO_DASH = 92;  /* スタート時、一時的に前倒し */
const int   Task::TAIL_ANGLE_WALK_BY       = 85;  /* 尻尾走行の角度[度] */
const int   Task::TAIL_ANGLE_DRIVE         =  3;  /* バランス走行時の角度[度] */

const int   Task::TAIL_ANGLE_CALIBRATION   = 95;


Task::Task(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : mNavigator(navigator),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false) {
      mSubState=0;
}

int Task::init()
{
    return 0;
}

int Task::run()
{
   return 0; 
}

int Task::tail_control( signed int angle )
{
     float pwm = (float)(angle - mTail.getCount() )*P_GAIN; /* 比例制御 */
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
        mTail.stop();
        return 1; // End of Moving Tail
    }
    else
    {
        mTail.setPWM((int)pwm);
    }
    return 0; // Must Continue (Not End of Moving Tail)
}

int Task::tail_control( signed int angle, float gain )
{
     float pwm = (float)(angle - mTail.getCount() )*gain; /* 比例制御 */
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
        mTail.stop();
        return 1; // End of Moving Tail
    }
    else
    {
        mTail.setPWM((int)pwm);
    }
    return 0; // Must Continue (Not End of Moving Tail)
}

void Task::setCurDirection(float curDirection){
    mCurDirection = curDirection;
}
