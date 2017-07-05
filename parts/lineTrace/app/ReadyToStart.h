#include "Motor.h"

class ReadyToStart {
public:
    ReadyToStart(const ev3api::Motor& tail);

    void run(signed int angle);

private:
    ev3api::Motor& mTail;
    bool mIsInitialized;

    void init();
};