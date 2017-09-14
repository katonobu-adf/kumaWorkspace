#include "StartToDash.h"

const int StartToDash::FIRST_STEP  =   50; //  0.2s
const int StartToDash::SECOND_STEP =  100; //  0.4s
const int StartToDash::THIRD_STEP  =  150; //  0.6s
const int StartToDash::FOURTH_STEP =  400; //  1.6s
const int StartToDash::FIFTH_STEP  = 1000; //  4.0s

StartToDash::StartToDash(
           Navigator       *navigator,
           BalancingWalker *balancingWalker,
           ev3api::Motor   &tail)
    : TaskHolder( navigator, balancingWalker, tail)
{
    callCount=0;
    mLogging = new Logging();
}

// Override
int StartToDash::run(){

    // Intitial Step
    if ( callCount == 0 ){
        mBalancingWalker->init();
        // Tail 前倒し
        tail_control(TAIL_ANGLE_START_TO_DASH,2);
    } 
    // before 1st Step
    else if ( callCount < FIRST_STEP ){
        // Blind Start to run (to Straight)
        speed = 45;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    }
    // before 2nd Step
    else if ( callCount < SECOND_STEP ){
        tail_control(TAIL_ANGLE_DRIVE,10);
        speed = 50;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    }
    // before 3rd Step
    else if ( callCount < THIRD_STEP ){
        tail_control(TAIL_ANGLE_DRIVE);
        speed = 50;
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    }
    // before 4th Step
    else if ( callCount < FOURTH_STEP ) {
        tail_control(TAIL_ANGLE_DRIVE);
        // 少しずつスピードアップ
        //if ( callCount%40 == 0 ){ speed++;}
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
    }
    // before 5th Step
    // FOURTH STEP
    else if ( callCount < FIFTH_STEP ) {
        tail_control(TAIL_ANGLE_DRIVE);
        // 少しずつスピードアップ
        //if ( callCount%40 == 0 ){ speed++;}
        mBalancingWalker->setCommand(speed, 0);
        // 倒立走行を行う
        mBalancingWalker->run();
        return 1;  // 3.0s elapsed from start
    }
    else if ( callCount >= FIFTH_STEP ){
        return 1;
    }

    callCount++;
    return 0;
}
