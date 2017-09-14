#ifndef   EMERGENCY_STOP_H
#define   EMERGENCY_STOP_H

#include "TaskHolder.h"

// extends TaskHolder
class EmergencyStop : public TaskHolder {
public:
    EmergencyStop(
           Navigator * navigator,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
    int stop();
};


#endif    /* EMERGENCY_STOP_H */