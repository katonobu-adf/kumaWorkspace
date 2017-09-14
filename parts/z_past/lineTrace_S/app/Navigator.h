
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "TouchSensor.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class Navigator {
public:
    Navigator(const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::TouchSensor & touchSensor );
    void setBtCommand(int command);
    int  getBtCommand(void);
    bool isTouchPressed(void);
    void setState(int state);
    int  getState(void);

// 佐々木追加<begin>
    bool isOutputRequest();
    char *getMsg();
    void setMsg(int val);
// 佐々木追加<end>

private:
    int   mState;
    const LineMonitor*   mLineMonitor;
    BalancingWalker*     mBalancingWalker;
    ev3api::TouchSensor & mTouchSensor;
    int mBtCommand;

// 佐々木追加 <begin>
    char msg[128];
    bool request=false;

// 佐々木追加 <end>
};

#endif  /* NAVIGATOR_H */
