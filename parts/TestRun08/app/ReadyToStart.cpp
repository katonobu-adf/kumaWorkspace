#include "ReadyToStart.h"

ReadyToStart::ReadyToStart(
           Navigator * navigator,
           LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail)
{
    ;
}

// Override
int ReadyToStart::run(){

    int tailState=tail_control(TAIL_ANGLE_STAND_UP);
    
    if ( tailState == 1 ){
        if ( mNavigator->isTouchPressed() ){
            ev3_speaker_play_tone(NOTE_C4, 200);
            return 1;
        }
        if ( mNavigator->getBtCommand() == 1 ){
            ev3_speaker_play_tone(NOTE_C5, 200);
            return 1;
        }
    }
    return 0;
}
