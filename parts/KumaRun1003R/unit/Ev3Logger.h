/******************************************************************************
 *  Ev3Logger.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2017/07/17
 *  Implementation of the Class for Logging Sensor Output
 *  Author: Nobuhiko.Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef EV3_UNIT_EV3LOGGER_H_
#define EV3_UNIT_EV3LOGGER_H_

#include <string.h>

#include "TouchSensor.h"
#include "SonarSensor.h"
#include "ColorSensor.h"
#include "GyroSensor.h"
#include "Motor.h"
#include "Clock.h"

#define DATA_LEN 81
// #define HEADER "time(msec),Color(Reflect),Color(Ambient),Gyro(deg/sec),Gyro(deg),UltraSonic(cm),Touch,Tail,LeftWheel,RightWheel,Battery(mV)\r\n"
#define HEADER "time(msec),Color(Reflect/GrayScale),Gyro(deg/sec),UltraSonic(cm),Touch,Tail,LeftWheel,RightWheel,Battery(mV),state,forward,turn,active_mode\r\n"

class Ev3Logger {
public:
    Ev3Logger(const ev3api::ColorSensor& colorSensor,
                    ev3api::GyroSensor& gyroSensor,
                    ev3api::TouchSensor& touchSensor,
                    ev3api::SonarSensor& sonarSensor,
                    ev3api::Motor& tail,
                    ev3api::Motor& leftWheel,
                    ev3api::Motor& rightWheel);
                    // ev3api::Clock& clock);

    void logging(int state, int forward, int turn, int active_mode, int grayScaleValue);

    void resetTime();

private:
    const ev3api::ColorSensor mColorSensor;
    ev3api::GyroSensor mGyroSensor;
    ev3api::TouchSensor mTouchSensor;
    ev3api::SonarSensor mSonarSensor;
    ev3api::Motor mTail;
    ev3api::Motor mLeftWheel;
    ev3api::Motor mRightWheel;


    // int8_tだとRGBモードでオーバーフローする
    // int8_t colorBrightnessValue = 0;
    uint16_t colorBrightnessValue = 0;
    // uint8_t colorAmbientValue = 0;
    int16_t gyroAngleVelocity = 0;
    // int16_t gyroAngle = 0;
    uint16_t sonarDistance = 0;
    bool touched = false;
    int32_t tailCount = 0;
    int32_t rightCount = 0;
    int32_t leftCount = 0;
    int batteryVoltage = 0;

    char datas[DATA_LEN + 1];

    ulong_t timeOffset = 0;

    ulong_t getTime();
};

#endif  // EV3_UNIT_EV3LOGGER_H_
