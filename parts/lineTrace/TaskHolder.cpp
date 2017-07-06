
#include "TaskHolder.h"

TaskHolder::TaskHolder()
{
    
}

int TaskHolder::run()
{
   return 0; 
}

TaskHolder::tail_control(signed int angle )
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
    }
    else
    {
        mTail.setPWM((int)pwm);
    }
}
