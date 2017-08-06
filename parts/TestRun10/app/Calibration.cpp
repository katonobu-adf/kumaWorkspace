
/******************************************************************************
 *  Calibration.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Calibration.h"

Calibration::Calibration(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator, balancingWalker, tail)
{
    mLogging = new Logging();
}

// Override
int Calibration::run(){

    if ( mNavigator->isTouchPressed() ){
        return 1;
    }
    if ( mNavigator->getBtCommand() == 1 ){
        return 1;
    }
 
    // 現在の明るさを求める
    //int   brightness    = mLineMonitor->getBrightness();  
    // 平均の明るさを求める
    //float avgBrightness = mLineMonitor->getAverageBrightness();
    // sprintf(buf, "%3d / %5.2f", brightness, avgBrightness );
    // mLogging->send(buf);

    // 何もしないで終わる
    return 1;
    //return 0;
}
