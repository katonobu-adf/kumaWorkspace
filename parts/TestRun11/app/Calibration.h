
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

// 佐々木 <begin>
    // 今後メインで使うメソッド
    void setWhite();
    void setBlack();
    void confirmation();
    // 使う予定のないメソッド（残しているだけ）
    void dispAmbient();
    void dispBrightness();
    void checkCalibration();
    
    // 処理切り替え用
    int toggle = 0;         // キャリブレートの段階表す変数
    int interval = 0;       // クールダウンのタイマー変数

    // ディスプレイでの表示をわかりやすくするための変数
    static const int LINE_SPACING; // 表示する時の、行の間隔を設定

    // dispBrightness で使用（今後使う予定のないメソッド）
    int configBrightness;   // 取得したい明るさを指定

// 佐々木 <end>
};


#endif    /* CARIBRATION_H */