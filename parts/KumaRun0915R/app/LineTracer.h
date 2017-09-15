
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

    static const int LOW;   // KC=2.80 TC=0.444
    static const int SECOND;   // KC=1.50 TC=0.588
    static const int THIRD;   // KC=2_00 TC=0.660
    static const int TOP;   // KC=1.70 TC=0.732
    static const int HIGHTOP;  // KC=1.20 TC=0.792

    float e_t;                 /* 偏差 */
    float der_e_t = 0;         /* 偏差の微分項 */
    float int_e_t = 0;         /* 偏差の積分項 */
    float prev_e_t= 0;         /* 1 ステップ前の偏差 */
    float u_t;                 /* 操作量計算領域（実数） */
    int   temp_turn;           /* 操作量計算領域（整数） */
    int   temp_forward;
    unsigned short int m_val;  /* 観測値 */

//  灰色マーカ検知のしきい値
    static const float  GRAY_BAND_MIN;
    static const float  GRAY_BAND_MAX;

    // ADF)加藤
    // 走行距離による別タスク移行のしきい値
    static const int RUNNING_DISTANCE_THRESHOLD;
    // ADF)加藤

// ADF)加藤
// 継承先で使うメンバーとメソッドを移動
protected:
    // しきい値と速度に応じて、走行体の方向を決める。
    int  calcDirection(int brightness, int forward);
    int  abs(int num);

    // PID格納用テーブル(ここから速度に対応したPIDを選択する
    PID **pid;

    // ログ取得用
    Logging* mLogging;
// ADF)加藤

private:
    // 最適なスピードを計算する
    int  calcForward(int turn, int forward);
    int calcControler(int forward, int gear);
    
    // 一応残しておく (元の引数1バージョン)
    // int calcDirection(int brightness);
    
    // 灰色マーカを検知したか
    bool isOnGrayMark(int brightness, int avgBrightness);


    int callCount;
    int mAvgCnt;
    int mGrayThreshold;

private:
};

#endif  // LINETRACER_H_
