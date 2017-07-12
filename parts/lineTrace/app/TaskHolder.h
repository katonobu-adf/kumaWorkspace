#ifndef TASKHOLDER_H
#define TASKHOLDER_H

#include "ev3api.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class TaskHolder {
public:
    static const float P_GAIN;              /* 完全停止用モータ制御比例係数 */
    static const int PWM_ABS_MAX;           /* 完全停止用モータ制御PWM絶対最大値 */
    static const int TAIL_ANGLE_STAND_UP;   /* 完全停止時の角度[度] */
    static const int TAIL_ANGLE_WALK_BY;    /* 尻尾走行の角度[度] */
    static const int TAIL_ANGLE_DRIVE;      /* バランス走行時の角度[度] */

    TaskHolder(
           Navigator * navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int init();
    virtual int run();

protected:
    Navigator * mNavigator;
    const LineMonitor * mLineMonitor;
    BalancingWalker* mBalancingWalker;
    ev3api::Motor & mTail;
    bool mIsInitialized;

protected:
    int  tail_control(signed int angle);
    int  tail_control(signed int angle, float gain);
    int  subState;
};


#endif  /* TASKHOLDER_H */
