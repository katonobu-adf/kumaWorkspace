
/******************************************************************************
 *  Calibration.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Calibration.h"

Calibration::Calibration(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : Task(navigator, balancingWalker, tail)
{
    mLogging = new Logging();
}

// Override
int Calibration::run(){
    
    if(mNavigator->isTouchPressed() ||
        mNavigator->getBtCommand() == 1)
    {
        if(interval <= 0){
            ++toggle;       // 次の段階へ(２で終了)
            interval = 250; // クールダウンへ(4ms * 250 = 1s)
        }
    }
    
    switch(toggle){
        case 0:
            dispAmbient();      // 環境光採取
            break;
        case 1:
            setWhite();   // 白の値の採取
            break;
        case 2:
            setBlack();   // 黒の値の採取
            break;
        case 3:
            confirmation(); // キャリブレートの確認
            break;
        default:
            sprintf(buf, "Start!");
            ev3_lcd_draw_string( buf, 0, 90);
            return 1;
            break;
    }
    if(interval > 0){
        --interval;         // もしクールダウン中ならカウントを減らす
    }
    
    return 0;
}
void Calibration::dispAmbient(){
    
    // 現在の環境光を測る
    int   ambient    = mNavigator->getAmbient();  

    sprintf(buf, "Ambient:%3d", ambient );
    ev3_lcd_draw_string( buf, 0, 15);
}

void Calibration::setWhite(){

    // 白の明るさを測る
    white = mNavigator->getBrightness();  
    sprintf(buf, "White:%3d", white );
    ev3_lcd_draw_string( buf, 0, 45);

    mNavigator->setWhite(white);
}

void Calibration::setBlack(){
    
    // 黒の明るさを測る
    black = mNavigator->getBrightness();  
    sprintf(buf, "Black:%3d", black );
    ev3_lcd_draw_string( buf, 0, 60);

    mNavigator->setBlack(black);
}


void Calibration::confirmation(){
    
    // 確認
    ev3_led_set_color(LED_ORANGE);
    sprintf(buf, "OK?");
    ev3_lcd_draw_string( buf, 0, 90);

    // 灰色の値を表示する
    mNavigator->setGray(black + 25);                    // 黒の値に任意の値を足して灰色の値として記録する
    sprintf(buf, "Gray:%3d", mNavigator->getGray());
    ev3_lcd_draw_string( buf, 30, 90);


    // 黒の閾値を計算しディスプレイ表示
    int8_t threshold = ( 13 * (white + mNavigator->getGray()) ) / 33; // 任意の計算方法でしきい値を出す

    mNavigator->setThreshold(threshold);
    sprintf(buf, "Threshold:%3d", mNavigator->getThreshold());
    ev3_lcd_draw_string( buf, 0, 105);
    
    // やり直し
    if(ev3_button_is_pressed(DOWN_BUTTON)){
        sprintf(buf, "ReS");
        ev3_lcd_draw_string( buf, 0, 90);
        toggle = 0;

    }
}





// 試走会２で使わないもの
void Calibration::dispBrightness(){

    // 目標の明るさを設定する
    if(interval <= 0){
        if(ev3_button_is_pressed(UP_BUTTON)){
            ++configBrightness;
            interval = 50; // クールダウン発生

        }else if(ev3_button_is_pressed(DOWN_BUTTON)){
            --configBrightness;
            interval = 50; // クールダウン発生
        }
    }

    // 横に表示する
    sprintf(buf, "config:%3d", configBrightness);
    ev3_lcd_draw_string(buf, 100, 30);

    // 現在の環境光を求める
    int   brightness    = mNavigator->getBrightness();  
    sprintf(buf, "Brightness:%3d", brightness );
    ev3_lcd_draw_string( buf, 0, 30);

    if(brightness == configBrightness){
        ev3_led_set_color(LED_GREEN);

    }else{
        ev3_led_set_color(LED_ORANGE);
    }
}

void Calibration::checkCalibration(){
    
    // 現在の反射光を計測
    int   brightness    = mNavigator->getBrightness();  
    sprintf(buf, "Brightness:%3d", brightness );
    ev3_lcd_draw_string( buf, 0, 75);
    
    // キャリブレーションが上手くいっているか
    int calibratedValue = mNavigator->calibrate(brightness);
    sprintf(buf, "calibrate:%3d", calibratedValue );
    ev3_lcd_draw_string( buf, 100, 75);
}

