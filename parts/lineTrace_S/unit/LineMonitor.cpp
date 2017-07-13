/******************************************************************************
 *  LineMonitor.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineMonitor
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineMonitor.h"

// �萔�錾
const int8_t LineMonitor::INITIAL_THRESHOLD = 20;  // ���F�̌��Z���T�l

/**
 * �R���X�g���N�^
 * @param colorSensor �J���[�Z���T
 */
LineMonitor::LineMonitor(const ev3api::ColorSensor& colorSensor)
    : mColorSensor(colorSensor),
      mThreshold(INITIAL_THRESHOLD) {
}

/**
 * ���C���ォ�ۂ��𔻒肷��
 * @retval true  ���C����
 * @retval false ���C���O
 */
bool LineMonitor::isOnLine() const {
    // ���Z���T����̎擾�l���
    // ���ȏ�ł���΁utrue�v��A
    // �����łȂ���΁ufalse�v��Ԃ�
    if (mColorSensor.getBrightness() >= mThreshold) {
        return true;
    } else {
        return false;
    }
}

/**
 * ���C��臒l��ݒ肷��
 * @param threshold ���C��臒l
 */
void LineMonitor::setThreshold(int8_t threshold) {
    mThreshold = threshold;
}

void LineMonitor::setCalibrateBlack(int black){
    mCalibrateBlack = black;
}

void LineMonitor::setCalibrateWhite(int white){
    mCalibrateWhite = white;
}

int  LineMonitor::getCalibrateBlack() const{
    return mCalibrateBlack;
}

int  LineMonitor::getCalibrateWhite() const{
    return mCalibrateWhite;
}

// ���R�ǉ� <begin>
int8_t LineMonitor::getThreshold() const{
    return mThreshold;
}

int LineMonitor::getBrightness() const {
    return mColorSensor.getBrightness();
}

int LineMonitor::getNormalizeBrightness() const{
    normalizeBrightness =
      ((mColorSensor.getBrightness() - mCalibrateWhite) / ( mCalibrateBlack - mCalibrateWhite)) * 100;

    return normalizeBrightness;
}
// ���R�ǉ� <end>