
/******************************************************************************
 *  Calibration.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Calibration.h"

const int Calibration::LINE_SPACING = 15;   // 行間隔を設定(縦方向)
const int Calibration::INTERVAL     = 250;  // タッチセンサーを押したときのクールダウンの時間
const int Calibration::ADD_GRAY     = 25;   // 灰色の明るさに近づかせるため、黒の値に足す値

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

    // キャリブレーションを最初からやり直す
    if(ev3_button_is_pressed(DOWN_BUTTON)){
        sprintf(buf, "ReS");
        ev3_lcd_draw_string( buf, 0, 90);
        toggle = 0;

    }


    // キャリブレーションを進める
    if(mNavigator->isTouchPressed() ||
        ev3_button_is_pressed(ENTER_BUTTON))
    {
        // クールダウンが発生していないか？
        if(interval <= 0){
            ++toggle;       // 次の処理へ移る
            interval = INTERVAL; // 1秒のクールダウン発生

        }
    }
    tail_control(TAIL_ANGLE_CALIBRATION);
    
    // toggleにより処理を開始する
    switch(toggle){
        case 0:
            dispAmbient();  // 環境光の採取
            break;    
        case 1:
            setWhite();     // 白の明るさの採取
            break;
        case 2:
            setBlack();     // 黒の明るさの採取
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

/*
 * run() で使うメソッド
 */

// 環境光を測定する（今のところ値の利用はしない予定）
void Calibration::dispAmbient(){
    
    // 現在の環境光を測る
    int ambient = mNavigator->getAmbient();  
    sprintf(buf, "Ambient:%3d", ambient );
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 3);

}

// 白の明るさを測定し、Navigatorへ渡す
void Calibration::setWhite(){

    int white = mNavigator->getBrightness();
    sprintf(buf, "White:%3d", white);
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 1);

    mNavigator->setWhite(white);
}

// 黒の明るさを測定し、Navigatorへ
void Calibration::setBlack(){
    
    int black = mNavigator->getBrightness();
    sprintf(buf, "Black:%3d", black);
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 2);

    mNavigator->setBlack(black);
}

// 白と黒の明るさの値を用いて、他の値を計算する（灰色、しきい値など）
void Calibration::confirmation(){

    /*
     *変更点１: black の値に いくらの値を足して灰色の値として記録するか
     */
    int gray = mNavigator->getBlack() + ADD_GRAY; // 黒の値に任意の値を足して、灰色の値として記録する

    mNavigator->setGray(gray);
    sprintf(buf, "Gray:%3d", mNavigator->getGray());
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 4);

    /*
     *変更点？: 今は白と灰色の合計の 39% で今後は変える可能性があるかも
     */
    
    // Rコース用
    // int8_t threshold = ( 9 * (mNavigator->getWhite() + mNavigator->getGray()) ) / 20; // 白と灰色の値の合計を任意の値で割る(今は 45% の値をしきい値に)

    // Lコース用
    int8_t threshold = ( 7 * (mNavigator->getWhite() + mNavigator->getGray()) ) / 20; // 白と灰色の値の合計を任意の値で割る(今は 35% の値をしきい値に)

    mNavigator->setThreshold(threshold);
    sprintf(buf, "Threshold:%3d", mNavigator->getThreshold());
    ev3_lcd_draw_string( buf, 50, LINE_SPACING * 4);


    // 値を確認する
    sprintf(buf, "OK?");
    ev3_lcd_draw_string( buf, 0, LINE_SPACING * 5);    
}



/*
 * 使う予定がないメソッド
 */

// 反射光を測り、設定した値を取得したら反応を返すもの
void Calibration::dispBrightness(){

    // 取得したい明るさを指定
    if(interval <= 0){
        if(ev3_button_is_pressed(UP_BUTTON)){
            ++configBrightness;     // 設定値を上げる
            interval = 50;          // 0.1秒クールダウン

        }else if(ev3_button_is_pressed(DOWN_BUTTON)){
            --configBrightness;     // 設定値を下げる
            interval = 50;          // 0.1秒クールダウン
        }
    }
    
    // 表示位置を設定
    sprintf(buf, "config:%3d", configBrightness);
    ev3_lcd_draw_string(buf, 100, 30);

    // 現在の環境光を求める
    int   brightness    = mNavigator->getBrightness();  
    sprintf(buf, "Brightness:%3d", brightness );
    ev3_lcd_draw_string( buf, 0, 30);

    // 設定値と同じなら LED で知らせる
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
    
    // キャリブレーションが上手くいっているかを確認する
    int calibratedValue = mNavigator->calibrate(brightness);
    sprintf(buf, "calibrate:%3d", calibratedValue );
    ev3_lcd_draw_string( buf, 100, 75);
}

