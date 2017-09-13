
/******************************************************************************
 *  StartToDash.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

 #include "StartToDash.h"

const int StartToDash::FIRST_STEP  =   50; //  0.20s
const int StartToDash::SECOND_STEP =  100; //  0.40s
const int StartToDash::THIRD_STEP  =  150; //  0.60s
const int StartToDash::FOURTH_STEP =  400; //  1.60s
const int StartToDash::FIFTH_STEP  =  750; //  3.00s

const int FIRST_SPEED = 40;
const int SECOND_SPEED = 40;
const int THIRD_SPEED = 42;
const int FOURTH_SPEED = 44;
const int FIFTH_SPEED = 56;

StartToDash::StartToDash(
           Navigator       *navigator,
           BalancingWalker *balancingWalker,
           ev3api::Motor   &tail)
    : Task( navigator, balancingWalker, tail)
{
    callCount = 0;
}

int StartToDash::run(){

    // Intitial Step
    if ( callCount == 0 ) {
        // Tail 前倒し
        tail_control(TAIL_ANGLE_START_TO_DASH, 2);
    } else if ( callCount < FIRST_STEP ) {
        // before 1st Step
        speed = FIRST_SPEED;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    } else if ( callCount < SECOND_STEP ) {
        // before 2nd Step
        tail_control(TAIL_ANGLE_DRIVE,10);
        speed = SECOND_SPEED;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    } else if ( callCount < THIRD_STEP ) {
        // before 3rd Step
        tail_control(TAIL_ANGLE_DRIVE);
        speed = THIRD_SPEED;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    } else if ( callCount < FOURTH_STEP ) {
        // before 4th Step
        tail_control(TAIL_ANGLE_DRIVE);
        speed = FOURTH_STEP;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    } else if ( callCount < FIFTH_STEP ) {
        // before 5th Step
        // FOURTH STEP
        tail_control(TAIL_ANGLE_DRIVE);
        speed = FIFTH_SPEED;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
        return 1;  // 3.0s elapsed from start
    } else if ( callCount >= FIFTH_STEP ){
        return 1;
    }

    callCount++;
    return 0;
}
