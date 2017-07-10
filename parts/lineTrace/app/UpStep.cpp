/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "UpStep.h"

// ���R�ǉ� <begin>

// const float LineTracer::INTERVAL = 0.004;  /* ����Ԋu 4 [ms] */
// const int   LineTracer::TURN_MAX = 100;    /* ����ʂ̍ő�l */
// const int   LineTracer::TURN_MIN = (-100); /* ����ʂ̍ŏ��l */
// const float LineTracer::KC = 3;            /* ���E���x�@�ɂ�鎝���U�����̔��Q�C�� */
// const float LineTracer::TC = 0.484;        /* ���E���x�@�ɂ�鎝���U���̎��� */
// const float LineTracer::KP = (0.6 * KC);   /* ��ᓮ��̔��W�� */
// const float LineTracer::TI = (0.5 * TC);   /* �ϕ�����̔��W�� */
// const float LineTracer::TD = (0.125 * TC); /* ��������̔��W�� */

// ���R�ǉ� <end>
/**
 * �R���X�g���N�^
 * @param lineMonitor     ���C������
 * @param balancingWalker �|�����s
 */
UpStep::UpStep(
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
int UpStep::run() {
    // ���R
    // 10�b���C���g���[�X������ʂ̂��Ƃ��
    callCount++;
    if (callCount > 2500) {
        return 1;
    }

    // ���R
    
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }
    
    tail_control(TAIL_ANGLE_DRIVE);
    //int  brightness = mLineMonitor->getBrightness();

    // ���s�̂̌������v�Z����
    //int direction = calcDirection(brightness);

    mBalancingWalker->setCommand(BalancingWalker::LOW, 0);

    // �|�����s���s��
    mBalancingWalker->run();

    return 0;
}


// ���R�ǉ� <begin>

// ���R�ǉ� <end>

