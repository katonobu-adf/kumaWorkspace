
#include "TaskHolder.h"

const float TaskHolder::P_GAIN=2.5F;            /* 完全停止用モータ制御比例係数 */
const int   TaskHolder::PWM_ABS_MAX=60;         /* 完全停止用モータ制御PWM絶対最大値 */
const int   TaskHolder::TAIL_ANGLE_STAND_UP=92; /* 完全停止時の角度[度] */
const int   TaskHolder::TAIL_ANGLE_DRIVE=3;     /* バランス走行時の角度[度] */


TaskHolder::TaskHolder(
           Navigator* navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : mNavigator(navigator),
      mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false) {
    
}

int TaskHolder::init()
{
    return 0;
}

int TaskHolder::run()
{
   return 0; 
}

int TaskHolder::tail_control( signed int angle )
{
    // if ( angle <= mTail.getCount() ) {
    //     return 1;
    // }
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
        return 1;
    }
    else
    {
        mTail.setPWM((int)pwm);
    }
    return 0;
}
