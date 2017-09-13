/******************************************************************************
 *  BalancingWalker.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class BalancingWalker
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef BALANCINGWALKER_H_
#define BALANCINGWALKER_H_

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
    void stop(void);
    void blindWalk( int lPwmMotor, int rPwmMotor);
    int  brake( int brake );
    void setCommand(int forward, int turn);

    int   getRWheelOffset(void);
    int   getLWheelOffset(void);
    int   getSavedVelocity(void);

    int getForward();
    int getTurn();

private:
    const ev3api::GyroSensor& mGyroSensor;
    ev3api::Motor& mLeftWheel;
    ev3api::Motor& mRightWheel;
    Balancer* mBalancer;
    int mForward;
    int mTurn;
    int mVelocity;

    int mMode;
    int mRWheelCount;
    int mLWheelCount;
    int mRWheelOffset;
    int mLWheelOffset;

};

#endif  // BALANCINGWALKER_H_
