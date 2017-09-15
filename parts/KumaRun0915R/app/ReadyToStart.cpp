
/******************************************************************************
 *  ReadyToStart.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "ReadyToStart.h"

ReadyToStart::ReadyToStart(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : Task(navigator, balancingWalker, tail)
{
    ;
}

// Override
int ReadyToStart::run(){

    if(interval > 0){
        --interval;
    }
    if(ev3_button_is_pressed(UP_BUTTON)){
        if(interval <= 0){
            ++tail_adjust; // 尻尾の角度を深く
            interval = 50; // ０．１秒クールダウン
        }
    }
    if(ev3_button_is_pressed(DOWN_BUTTON)){
        if(interval <= 0){
            --tail_adjust; // 尻尾の角度を浅く
            interval = 50; // ０．１秒クールダウン
        }
    }

    int tail_angle = TAIL_ANGLE_STAND_UP + tail_adjust; // Taskクラスの定数を基準にして角度を決める
    sprintf(buf, "tail_angle: %d", tail_angle);
    ev3_lcd_draw_string(buf, 0, 75);
    
    int tailState=tail_control(tail_angle);
    mBalancingWalker->init();

    // スタートの準備の終わりを LED で知らせる
    ev3_led_set_color(LED_GREEN);
    
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
