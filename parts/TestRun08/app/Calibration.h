#ifndef   CALIBRATION_H
#define   CALIBRATION_H

#include "TaskHolder.h"
#include "Logging.h"

// extends TaskHolder
class Calibration : public TaskHolder {
public:
    Calibration(
           Navigator * navigator,
           LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run();
private:
    // ログ取得用
    Logging* mLogging;
    char buf[128];
};


#endif    /* CARIBRATION_H */