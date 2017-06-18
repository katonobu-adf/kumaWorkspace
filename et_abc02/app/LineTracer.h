/******************************************************************************
 *  LineTracer.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_APP_LINETRACER_H_
#define EV3_APP_LINETRACER_H_

#include "LineMonitor.h"
#include "BalancingWalker.h"

class LineTracer {
public:
    LineTracer(const LineMonitor* lineMonitor,
               BalancingWalker* balancingWalker);

    void run();
    void stop();
    void smoothStop();

private:
// ���R�ǉ� <begin>
/* PID ����p�@�}�N����` */
    static const float INTERVAL;   /* ����Ԋu 4 [ms] */
    static const int   TURN_MAX;   /* ����ʂ̍ő�l */
    static const int   TURN_MIN;   /* ����ʂ̍ŏ��l */
    static const float KC;         /* ���E���x�@�ɂ�鎝���U�����̔��Q�C�� */
    static const float TC;         /* ���E���x�@�ɂ�鎝���U���̎��� */
    static const float KP;         /* ��ᓮ��̔��W�� */
    static const float TI;         /* �ϕ�����̔��W�� */
    static const float TD;         /* ��������̔��W�� */

    /* PID ����p�@�ϐ��錾 */
    float e_t;           /* �΍� */
    float der_e_t=0;     /* �΍��̔����� */
    float int_e_t=0;     /* �΍��̐ϕ��� */
    float prev_e_t=0;    /* 1 �X�e�b�v�O�̕΍� */
    float u_t;           /* ����ʌv�Z�̈�i�����j */
    int temp_turn;       /* ����ʌv�Z�̈�i�����j */
    unsigned short int m_val;     /* �ϑ��l */
    

// ���R�ǉ� <end>

    const LineMonitor* mLineMonitor;
    BalancingWalker* mBalancingWalker;
    bool mIsInitialized;

    int calcDirection(bool isOnLine);
    
// ���R�ǉ� <begin> overload
    int calcDirection(int brightness);
// ���R�ǉ� <end>    
};

#endif  // EV3_APP_LINETRACER_H_
