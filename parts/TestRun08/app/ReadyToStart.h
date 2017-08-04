#ifndef   READYTOSTART_H
#define   READYTOSTART_H

#include "TaskHolder.h"

// extends TaskHolder
class ReadyToStart : public TaskHolder {
public:
    ReadyToStart(
           Navigator * navigator,
           LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
};


#endif    /* READYTOSTART_X_H */