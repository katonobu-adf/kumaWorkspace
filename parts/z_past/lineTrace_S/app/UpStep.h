/******************************************************************************
 *  LineTracer.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef UPSTEP_H_
#define UPSTEP_H_

#include "TaskHolder.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class UpStep : public TaskHolder{
public:
    UpStep(
           Navigator * navigator,
           const LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();

private:
// 奥山追加 <begin>
    /* PID 制御用　変数宣言 */
    float e_t;           /* 偏差 */
    float der_e_t=0;     /* 偏差の微分項 */
    float int_e_t=0;     /* 偏差の積分項 */
    float prev_e_t=0;    /* 1 ステップ前の偏差 */
    float u_t;           /* 操作量計算領域（実数） */
    int temp_turn;       /* 操作量計算領域（整数） */
    unsigned short int m_val;     /* 観測値 */

private:
    int callCount;
// 奥山追加 <end>    
};

#endif  // UPSTEP_H_
