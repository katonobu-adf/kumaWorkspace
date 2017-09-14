/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

// ���R�ǉ� <begin>

const float LineTracer::INTERVAL = 0.004;  /* ����Ԋu 4 [ms] */
const int   LineTracer::TURN_MAX = 100;    /* ����ʂ̍ő�l */
const int   LineTracer::TURN_MIN = (-100); /* ����ʂ̍ŏ��l */
const float LineTracer::KC = 3;            /* ���E���x�@�ɂ�鎝���U�����̔��Q�C�� */
const float LineTracer::TC = 0.484;        /* ���E���x�@�ɂ�鎝���U���̎��� */
const float LineTracer::KP = (0.6 * KC);   /* ��ᓮ��̔��W�� */
const float LineTracer::TI = (0.5 * TC);   /* �ϕ�����̔��W�� */
const float LineTracer::TD = (0.125 * TC); /* ��������̔��W�� */

// ���R�ǉ� <end>
/**
 * �R���X�g���N�^
 * @param lineMonitor     ���C������
 * @param balancingWalker �|�����s
 */
LineTracer::LineTracer(
           Navigator * navigator,
           const LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail) {
        callCount=0;
          ;
}

/**
 * ���C���g���[�X����
 */
int LineTracer::run() {
    // ���R
    // 10�b���C���g���[�X������ʂ̂��Ƃ��
    // callCount++;
    // if (callCount > 1000) {
    //     return 1;
    // }

    // ���R
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }
    
    tail_control(TAIL_ANGLE_DRIVE);
    int brightness = mLineMonitor->getBrightness();
    int normalBrightness = mLineMonitor->getNormalizeBrightness();
    int black = mLineMonitor->getCalibrateBlack();
    int white = mLineMonitor->getCalibrateWhite();

    sprintf(msgBuf,"Run:LineTrace %3d %3d %3d, ", normalBrightness, black, white);
	ev3_lcd_draw_string(msgBuf, 10, 0);

    mNavigator->setMsg(normalBrightness); // ファイルへ正規化輝度の数値を送る

    // ���s�̂̌�����v�Z����
    int direction = calcDirection(brightness);

    mBalancingWalker->setCommand(BalancingWalker::HIGH, direction);

    // �|�����s��s��
    mBalancingWalker->run();

    return 0;
}


// ���R�ǉ� <begin>
int LineTracer::calcDirection(int brightness){
    int turn;
    /* PID ����p�@����ʂ̎Z�o */
    /* �ϑ��l��擾 */
    e_t = (float)(mLineMonitor->getThreshold()- brightness); /* �΍���Z�o */

    int_e_t = int_e_t + e_t * INTERVAL; /* �ϕ�����Z�o */
    der_e_t = (e_t - prev_e_t) / INTERVAL; /* ��������Z�o */
    u_t = KP * (e_t + int_e_t / TI + TD * der_e_t); /* ����ʂ�v�Z */
    temp_turn = -(int)u_t; /* ����ʂ𐮐��� */

    if (temp_turn > TURN_MAX) /* ����ʂ̏���ݒ� */
        temp_turn = TURN_MAX;
    if (temp_turn < TURN_MIN) /* ����ʂ̉����ݒ� */
        temp_turn = TURN_MIN;
    
    turn = (signed char)temp_turn; /* ����ʂ�m�� */

    /* ���݂̕΍����� 1 �X�e�b�v�O�̕΍��Ƃ��� */
    prev_e_t = e_t;
    
    return turn;
}
// ���R�ǉ� <end>

