
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

    // 定数
    static const int LINE_SPACING;
    static const int INTERVAL;
    static const int ADD_GRAY;

    // 今後メインで使うメソッド
    void dispAmbient();
    void setWhite();
    void setBlack();
    void confirmation();

    // 処理切り替え用
    int toggle = 0;
    int interval = 0;
    

    // 使う予定のないメソッド（残しているだけ）
    void dispBrightness();
    void checkCalibration();

    // dispBrightness で使用（今後使う予定のないメソッド）
    int configBrightness;   // 取得したい明るさを指定
};

#endif    /* CARIBRATION_H */