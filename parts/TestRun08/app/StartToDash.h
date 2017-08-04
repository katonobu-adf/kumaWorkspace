#ifndef   START_TO_DASH_H
#define   START_TO_DASH_H

#include "TaskHolder.h"

// extends TaskHolder
class StartToDash : public TaskHolder {
public:
    StartToDash(
           Navigator * navigator,
           LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
private:
    static const int FIRST_STEP;
    static const int SECOND_STEP;
    static const int THIRD_STEP;
    static const int FOURTH_STEP;
    static const int FIFTH_STEP;
    
    int callCount;
    int speed;
};

#endif    /* START_TO_DASH_H */