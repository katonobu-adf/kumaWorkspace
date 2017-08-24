
/******************************************************************************
 *  Calibration.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef   CALIBRATION_H
#define   CALIBRATION_H

#include "Task.h"
#include "Logging.h"

// extends Task
class Calibration : public Task {
public:
    Calibration(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
private:
    // ログ取得用
    Logging* mLogging;
    char buf[128];

// 佐々木
    void dispAmbient();
    void setWhite();
    void setBlack();
    void checkCalibration();
    void confirmation();
    void dispBrightness();
    
    // 黒と白の値の保持
    int black;
    int white;

    // 目標の明るさを設定する変数
    int configBrightness;


    // 処理切り替え用
    int toggle = 0;   // 環境光と反射光採取の切り替え
    int interval = 0; // タッチセンサーを押したときのクールダウン

// 佐々木
};


#endif    /* CARIBRATION_H */