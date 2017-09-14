/******************************************************************************
 *  Ev3Logger.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2017/07/17
 *  Implementation of the Class for Logging Sensor Output
 *  Author: Nobuhiko.Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/


#include "Ev3Logger.h"
#include <syssvc/serial.h>

/**
 * コンストラクタ
 */
Ev3Logger::Ev3Logger(const ev3api::ColorSensor& colorSensor,
                    ev3api::GyroSensor& gyroSensor,
                    ev3api::TouchSensor& touchSensor,
                    ev3api::SonarSensor& sonarSensor,
                    ev3api::Motor& tail,
                    ev3api::Motor& leftWheel,
                    ev3api::Motor& rightWheel)
    : mColorSensor(colorSensor),
      mGyroSensor(gyroSensor),
      mTouchSensor(touchSensor),
      mSonarSensor(sonarSensor),
      mTail(tail),
      mLeftWheel(leftWheel),
      mRightWheel(rightWheel) {
}

/**
 * ロギングを行う
 */
void Ev3Logger::logging(int state, int forward, int turn) {

    colorBrightnessValue = mColorSensor.getBrightness();
    // colorAmbientValue = mColorSensor.getAmbient();

    gyroAngleVelocity = mGyroSensor.getAnglerVelocity();
    // gyroAngle = mGyroSensor.getAngle();

    sonarDistance = mSonarSensor.getDistance();

    touched = mTouchSensor.isPressed();

    tailCount = mTail.getCount();
    leftCount = mLeftWheel.getCount();
    rightCount = mRightWheel.getCount();

    batteryVoltage = ev3_battery_voltage_mV ();
        
    ulong_t time = getTime() - timeOffset;
    // sprintf(datas, "%ld,%d,%d,%d,%d,%d,%d,%ld,%ld,%ld,%d,%d\r\n", 
    sprintf(datas, "%ld,%d,%d,%d,%d,%ld,%ld,%ld,%d,%d,%d,%d\r\n", 
        time, 
        colorBrightnessValue, 
        // colorAmbientValue,
        gyroAngleVelocity,
        // gyroAngle,
        sonarDistance,
        touched,
        tailCount,
        leftCount,
        rightCount,
        batteryVoltage,
        state,
        forward,
        turn);

    serial_wri_dat(SIO_PORT_BT, datas, strlen(datas));
}


void Ev3Logger::resetTime()
{
    timeOffset = getTime();
    serial_wri_dat(SIO_PORT_BT, HEADER, strlen(HEADER));
}


ulong_t Ev3Logger::getTime()
{
    static ulong_t start = -1;
    ulong_t time;

    get_tim(&time);

    if(start < 0)
    {
        start = time;
    }

    return time - start;
}
