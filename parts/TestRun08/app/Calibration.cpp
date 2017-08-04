#include "Calibration.h"

Calibration::Calibration(
           Navigator * navigator,
           LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail)
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
