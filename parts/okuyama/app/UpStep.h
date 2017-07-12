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
// ���R�ǉ� <begin>
    /* PID ����p�@�ϐ��錾 */
    float e_t;           /* �΍� */
    float der_e_t=0;     /* �΍��̔����� */
    float int_e_t=0;     /* �΍��̐ϕ��� */
    float prev_e_t=0;    /* 1 �X�e�b�v�O�̕΍� */
    float u_t;           /* ����ʌv�Z�̈�i�����j */
    int temp_turn;       /* ����ʌv�Z�̈�i�����j */
    unsigned short int m_val;     /* �ϑ��l */

private:
    int callCount;
// ���R�ǉ� <end>    
};

#endif  // UPSTEP_H_
