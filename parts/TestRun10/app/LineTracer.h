
/******************************************************************************
 *  LineTracer.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef LINETRACER_H_
#define LINETRACER_H_

#include "Task.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "PID.h"
#include "Logging.h"

class LineTracer : public Task{
public:
    LineTracer(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();

private:

// PID 制御用　マクロ定義
    static const float INTERVAL;
    static const int   TURN_MAX;
    static const int   TURN_MIN;

    float e_t;                 /* 偏差 */
    float der_e_t = 0;         /* 偏差の微分項 */
    float int_e_t = 0;         /* 偏差の積分項 */
    float prev_e_t= 0;         /* 1 ステップ前の偏差 */
    float u_t;                 /* 操作量計算領域（実数） */
    int   temp_turn;           /* 操作量計算領域（整数） */
    unsigned short int m_val;  /* 観測値 */

//  灰色マーカ検知のしきい値
    static const float  GRAY_BAND_MIN;
    static const float  GRAY_BAND_MAX;

private:
    // 最適なスピードを計算する
    int  calcForward(int turn);
    
    // 一応残しておく (元の引数1バージョン)
    // int calcDirection(int brightness);
    
    // しきい値と速度に応じて、走行体の方向を決める。
    int  calcDirection(int brightness, int forward);

    // 灰色マーカを検知したか
    bool isOnGrayMark(int brightness, int avgBrightness);

    int  abs(int num);

    int callCount;
    int mAvgCnt;
    int mGrayThreshold;

    // PID格納用テーブル(ここから速度に対応したPIDを選択する
    PID **pid;

private:
    // ログ取得用
    Logging* mLogging;
};

#endif  // LINETRACER_H_
