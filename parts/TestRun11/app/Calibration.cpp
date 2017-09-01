
/******************************************************************************
 *  Calibration.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Calibration.h"

const int Calibration::LINE_SPACING = 15; // 行間隔を設定(縦方向)

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

    // やり直す
    if(ev3_button_is_pressed(DOWN_BUTTON)){
        sprintf(buf, "ReS");
        ev3_lcd_draw_string( buf, 0, LINE_SPACING * 0);
        toggle = 0;

    }
    // キャリブレーションを進める
    if(mNavigator->isTouchPressed() ||
        mNavigator->getBtCommand() == 1)
    {
        // クールダウンが発生していないか？
        if(interval <= 0){
            ++toggle;       // 次の処理へ移る
            interval = 250; // 1秒のクールダウン発生
        }

    }

    tail_control(TAIL_ANGLE_CALIBRATION);
    
    // toggleにより処理を開始する
    switch(toggle){
        case 0:
            dispAmbient();
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
            // スタートの準備が終わったことを知らせる
            sprintf(buf, "Start!");
            ev3_lcd_draw_string( buf, 0, LINE_SPACING * 0);
            
            return 1;
            break;
    }

    if(interval > 0){
        --interval;         // クールダウンのカウントを減らす

    }
    return 0;
}


// 白の値を測定し、Navigatorへ渡す
void Calibration::setWhite(){

    int white = mNavigator->getBrightness();
    sprintf(buf, "White:%3d", white);
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 1);

    mNavigator->setWhite(white);
}

// 黒の値を測定し、Navigatorへ
void Calibration::setBlack(){
    
    int black = mNavigator->getBrightness();
    sprintf(buf, "Black:%3d", black);
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 2);

    mNavigator->setBlack(black);
}

// 白と黒の値で計算した、他の値の確認（灰色、しきい値など）
void Calibration::confirmation(){

    /*
     *変更点１: black の値に いくらの値を足して灰色の値として記録するか
     *
     *灰色の値を計算し記録する
     *
     */
    int gray = mNavigator->getBlack() + 25; // 黒の値に任意の値を足して、灰色の値として記録する

    mNavigator->setGray(gray);
    sprintf(buf, "Gray:%3d", mNavigator->getGray());
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 4);

    // LineTracer で利用するしきい値の表示
    int8_t threshold = ( 13 * (mNavigator->getWhite() + mNavigator->getGray()) ) / 33; // 白と灰色の値の合計を任意の値で割って決める
    mNavigator->setThreshold(threshold);
    sprintf(buf, "Threshold:%3d", mNavigator->getThreshold());
    ev3_lcd_draw_string( buf, 50, LINE_SPACING * 4);


    // 値を確認する
    sprintf(buf, "OK?");
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 5);    
}





// 試走会２で使わないもの

void Calibration::dispAmbient(){
    
    // 現在の環境光を測る
    int ambient = mNavigator->getAmbient();  

    sprintf(buf, "Ambient:%3d", ambient );
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 3);
}

void Calibration::dispBrightness(){

    // 取得したい明るさを指定
    if(interval <= 0){
        if(ev3_button_is_pressed(UP_BUTTON)){
            ++configBrightness;
            interval = 50; // 0.1秒クールダウン
        }else if(ev3_button_is_pressed(DOWN_BUTTON)){
            --configBrightness;
            interval = 50; // 0.1秒クールダウン

        }
    }
    // 表示位置を設定
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
    int brightness = mNavigator->getBrightness();  
    
    sprintf(buf, "Brightness:%3d", brightness );
    ev3_lcd_draw_string( buf, 0, 75);
    
    // キャリブレーションが上手くいっているか
    int calibratedValue = mNavigator->calibrate(brightness);

    sprintf(buf, "calibrate:%3d", calibratedValue );
    ev3_lcd_draw_string( buf, 100, 75);
}

