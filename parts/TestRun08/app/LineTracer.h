/******************************************************************************
 *  LineTracer.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef LINETRACER_H_
#define LINETRACER_H_

#include "TaskHolder.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "Logging.h"

class LineTracer : public TaskHolder{
public:
    LineTracer(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();

private:

// PID 制御用　マクロ定義
// PIDクラスへ転換
    static const float INTERVAL;   /* 制御間隔 4 [ms] */
    static const int   TURN_MAX;   /* 操作量の最大値 */
    static const int   TURN_MIN;   /* 操作量の最小値 */
    static const float KC;         /* 限界感度法による持続振動時の比例ゲイン */
    static const float TC;         /* 限界感度法による持続振動の周期 */
    static const float KP;         /* 比例動作の比例係数 */
    static const float TI;         /* 積分動作の比例係数 */
    static const float TD;         /* 微分動作の比例係数 */
// PID 制御用　変数宣言
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
    int calcDirection(int brightness);
    int callCount;
    int mAvgCnt;
    int mGrayThreshold;

private:
    // ログ取得用
    Logging* mLogging;
};

#endif  // LINETRACER_H_
