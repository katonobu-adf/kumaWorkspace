#ifndef  READYTOSTART_H
#define  READYTOSTART_H

#include "Motor.h"

class ReadyToStart {
public:
    ReadyToStart(ev3api::Motor& tail);
    void run(signed int angle);

private:
    ev3api::Motor mTail;
    bool mIsInitialized;

    void init();
};

#endif   /* READYTOSTART_H */
