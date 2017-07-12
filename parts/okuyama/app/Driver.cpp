
#include "Driver.h"

Driver::Driver(
           Navigator * navigator,
           const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : mNavigator(navigator),
      mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false)
{
    mTaskHolder[0] = new ReadyToStart( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    mTaskHolder[1] = new LineTracer( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    // mTaskHolder[2] = new Spin( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    
    // mTaskHolder[2] = new Spin();
    // mTaskHolder[3] = new Spin();
    // mTaskHolder[4] = new DansaDown();
}


int Driver::run()
{
    int ret;
    int state=mNavigator->getState();
    switch(state){
        case 0:
            ret=mTaskHolder[0]->run();
            if ( ret == 1 ){ mNavigator->setState(1);}
            break;
        case 1: // LineTrace
            ret=mTaskHolder[1]->run();
            if ( ret == 1 ){ mNavigator->setState(2);}
            break;
        // case 2: // Spin
        //     ret=mTaskHolder[2]->run();
        //     if ( ret == 1 ){ mNavigator->setState(3);}
        //     break;
        // case 3: // スピン
        //     mTaskHolder[3]->run();
        //     break;
        // case 4: // 段差 降りる
        //     mTaskHolder[4]->run();
        //     break;
        default:
            break;
    }
    return 0;
}