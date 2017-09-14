/******************************************************************************
 *  BalancingWalker.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class BalancingWalker
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_UNIT_BALANCINGWALKER_H_
#define EV3_UNIT_BALANCINGWALKER_H_

#include "GyroSensor.h"
#include "Motor.h"
#include "BalancerCpp.h"

class BalancingWalker {
public:
    static const int LOW;
    static const int SECOND;
    static const int THIRD;
    static const int TOP;
    static const int HIGHTOP;

    BalancingWalker(const ev3api::GyroSensor& gyroSensor,
                    ev3api::Motor& leftWheel,
                    ev3api::Motor& rightWheel,
                    Balancer* balancer);

    void init();
    void run();
    void blindWalk( int lPwmMotor, int rPwmMotor);
    int  brake( int brake );
    void setCommand(int forward, int turn);

// 奥山 追加
    void setSpinMode();
    void unsetSpinMode();
    int  spin(int round);
// 奥山 追加

private:
    const ev3api::GyroSensor& mGyroSensor;
    ev3api::Motor& mLeftWheel;
    ev3api::Motor& mRightWheel;
    Balancer* mBalancer;
    int mForward;
    int mTurn;

// 奥山 追加
    int mMode;
    int mRWheelCount;
    int mLWheelCount;
    int mRWheelOffset;
    int mLWheelOffset;
// 奥山 追加
};

#endif  // EV3_UNIT_BALANCINGWALKER_H_
