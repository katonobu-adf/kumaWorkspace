/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

/**
 * �R���X�g���N�^
 * @param lineMonitor     ���C������
 * @param balancingWalker �|�����s
 */
LineTracer::LineTracer(const LineMonitor* lineMonitor,
                       BalancingWalker* balancingWalker)
    : mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mIsInitialized(false) {
}

/**
 * ���C���g���[�X����
 */
void LineTracer::run() {
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }

    bool isOnLine = mLineMonitor->isOnLine();

    // ���s�̂̌������v�Z����
    int direction = calcDirection(isOnLine);

    mBalancingWalker->setCommand(BalancingWalker::LOW, direction);

    // �|�����s���s��
    mBalancingWalker->run();
}

/**
 * ���s�̂̌������v�Z����
 * @param isOnLine true:���C����/false:���C���O
 * @retval 30  ���C����ɂ���ꍇ(�E����w��)
 * @retval -30 ���C���O�ɂ���ꍇ(������w��)
 */
int LineTracer::calcDirection(bool isOnLine) {
    if (isOnLine) {
        // ���C����ɂ���ꍇ
        return BalancingWalker::LOW;
    } else {
        // ���C���O�ɂ���ꍇ
        return -BalancingWalker::LOW;
    }
}
