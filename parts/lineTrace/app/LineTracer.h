/******************************************************************************
 *  LineTracer.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_APP_LINETRACER_H_
#define EV3_APP_LINETRACER_H_

#include "TaskHolder.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "Logging.h"
#include "PID.h"

class LineTracer : public TaskHolder{
public:
    LineTracer(
           Navigator* navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail);
    int run();

private:
    static const float INTERVAL;
    static const int   TURN_MAX;
    static const int   TURN_MIN;

    float e_t;
    float der_e_t=0;
    float int_e_t=0;
    float prev_e_t=0;
    float u_t;
    int temp_turn;
    unsigned short int m_val;

private:
    int calcForward(int turn);
    int calcDirection(int brightness, int forward);
    int abs(int num);
    int callCount;
    Logging* mLogging;
    PID **pid;
// ‰œŽR’Ç‰Á <end>    
};

#endif  // EV3_APP_LINETRACER_H_
