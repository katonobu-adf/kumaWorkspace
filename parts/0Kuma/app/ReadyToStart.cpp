
/******************************************************************************
 *  ReadyToStart.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "ReadyToStart.h"

const int TONE_DURATION = 200;

ReadyToStart::ReadyToStart(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : Task(navigator, balancingWalker, tail)
{
    ;
}

int ReadyToStart::run(){

    mBalancingWalker->init();
    int tailState = tail_control(TAIL_ANGLE_STAND_UP);

    // スタートの準備の終わりを LED で知らせる
    ev3_led_set_color(LED_GREEN);

    if ( tailState == 1 ){
        if ( mNavigator->isTouchPressed() ){
            ev3_speaker_play_tone(NOTE_C4, TONE_DURATION);
            return 1;
        }
        if ( mNavigator->getBtCommand() == 1 ){
            ev3_speaker_play_tone(NOTE_C5, TONE_DURATION);
            return 1;
        }
    }
    return 0;
}
