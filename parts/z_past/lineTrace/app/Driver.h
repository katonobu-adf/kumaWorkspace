#ifndef  DRIVER_H
#define  DRIVER_H

#include "ev3api.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"
#include "TaskHolder.h"
#include "ReadyToStart.h"
#include "LineTracer.h"
#include "Spin.h"

class Driver {
public:
    Driver( Navigator * navigator,
            const LineMonitor* lineMonitor,
            BalancingWalker* balancingWalker,
            ev3api::Motor &tail);
    int run(void);

private:
    Navigator * mNavigator;
    const LineMonitor* mLineMonitor;
    BalancingWalker* mBalancingWalker;
    ev3api::Motor& mTail;
    bool mIsInitialized;

private:
    TaskHolder  * mTaskHolder[10];
};

#endif  /* DRIVER_H */
