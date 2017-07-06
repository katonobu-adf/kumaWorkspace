
#include "Driver.h"

Driver::Driver()
{
    mTaskHolder[0] = new ReadyToStart();
    mTaskHolder[1] = new DansaUp();
    mTaskHolder[2] = new Spin();
    mTaskHolder[3] = new Spin();
    mTaskHolder[4] = new DansaDown();
}


int Driver::run(int state)
{
    switch(state){
        case 0:
            mTaskHolder[0]->run();
            break;
        case 1: // LineTrace
            mTaskHolder[1]->run();
            break;
        case 2: // 段差 上る
            mTaskHolder[2]->run();
            break;
        case 3: // スピン
            mTaskHolder[3]->run();
            break;
        case 4: // 段差 降りる
            mTaskHolder[4]->run();
            break;
        default:
            break;
    }
}
