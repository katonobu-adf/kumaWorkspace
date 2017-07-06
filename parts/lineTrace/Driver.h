#ifndef  DRIVER_H
#define  DRIVER_H

#include "TaskHolder.h"
#include "ReadyToStartX.h"

class Driver {
public:
    Driver();
    int run(int state);
private:
    TaskHolder  * mTaskHolder[10];
};

#endif  /* DRIVER_H */
