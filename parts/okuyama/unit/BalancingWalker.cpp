/******************************************************************************
 *  BalancingWalker.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Class BalancingWalker
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "BalancingWalker.h"

// �萔�錾
const int BalancingWalker::LOW    = 30;    // �ᑬ
const int BalancingWalker::NORMAL = 50;    // �ʏ�
const int BalancingWalker::HIGH   = 70;    // ����

/**
 * �R���X�g���N�^
 * @param gyroSensor �W���C���Z���T
 * @param leftWheel  �����[�^
 * @param rightWheel �E���[�^
 * @param balancer   �o�����T
 */
BalancingWalker::BalancingWalker(const ev3api::GyroSensor& gyroSensor,
                                 ev3api::Motor& leftWheel,
                                 ev3api::Motor& rightWheel,
                                 Balancer* balancer)
    : mGyroSensor(gyroSensor),
      mLeftWheel(leftWheel),
      mRightWheel(rightWheel),
      mBalancer(balancer),
      mForward(LOW),
      mTurn(LOW),
      mMode(0) {
}

/**
 * �o�����X���s����
 */
void BalancingWalker::run() {
    int16_t angle = mGyroSensor.getAnglerVelocity();  // �W���C���Z���T�l
    int rightWheelEnc = mRightWheel.getCount();       // �E���[�^��]�p�x
    int leftWheelEnc  = mLeftWheel.getCount();        // �����[�^��]�p�x

    mBalancer->setCommand(mForward, mTurn);

    int battery = ev3_battery_voltage_mV();
    mBalancer->update(angle, rightWheelEnc, leftWheelEnc, battery);

    // ���E���[�^�ɉ�]���w������
    mLeftWheel.setPWM(mBalancer->getPwmLeft());
    mRightWheel.setPWM(mBalancer->getPwmRight());
}


/**
 * �o�����X���s�ɕK�v�Ȃ��̂����Z�b�g����
 */
void BalancingWalker::init() {
    int offset = mGyroSensor.getAnglerVelocity();  // �W���C���Z���T�l

    // ���[�^�G���R�[�_�����Z�b�g����
    mLeftWheel.reset();
    mRightWheel.reset();

    // �|���U�q���䏉����
    mBalancer->init(offset);
}

/**
 * PWM�l��ݒ肷��
 * @param forward �O�i�l
 * @param turn    ����l
 */
void BalancingWalker::setCommand(int forward, int turn) {
    mForward = forward;
    mTurn    = turn;
}


int BalancingWalker::brake(int brake){
    // int lPwm, rPwm;
    // lPwm = mLeftWheel.getPWM();
    // rPwm = mRightWheel.getPWM();

    mForward -= brake;

    return mForward;
}

void BalancingWalker::blindWalk(int lPwmMotor, int rPwmMotor){
    // ���E���[�^�ɉ�]���w������
    mLeftWheel.setPWM(  lPwmMotor );
    mRightWheel.setPWM( rPwmMotor );
}

void BalancingWalker::setSpinMode()
{
    mMode = 99;
    mRWheelOffset = mRightWheel.getCount();// �E���[�^��]�p�x
    mLWheelOffset = mRightWheel.getCount();// �����[�^��]�p�x
    mRWheelCount = 0;
    mLWheelCount = 0;
}
void BalancingWalker::unsetSpinMode()
{
    mMode = 0;
}
int BalancingWalker::spin( int round )
{
    mLWheelCount = mLeftWheel.getCount() - mLWheelOffset;
    mRWheelCount = mRightWheel.getCount() - mRWheelOffset;
    
    // Mode�`�F�b�N
    if ( mMode != 99 ) return -1;

    if ( mRWheelCount >= round || mLWheelCount <= -(round))
    {
        return 1; // End of Turn
    }
    mLeftWheel.setPWM(-10);
    mRightWheel.setPWM(10);
    return 0; // Must Continue (Not Ent of Turn)
}