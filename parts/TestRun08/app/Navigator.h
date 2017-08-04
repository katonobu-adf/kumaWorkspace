
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "TouchSensor.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class Navigator {
public:
    Navigator( const LineMonitor    *lineMonitor,
               BalancingWalker      *balancingWalker,
               ev3api::TouchSensor  &touchSensor );
    
    void   setBtCommand(int command);
    int    getBtCommand(void);
    bool   isTouchPressed(void);
    void   setState(int state);
    int    getState(void);
    void   recordVector(void);
    float  getVector(void);
    
    void   recordGyro(void);
    float  getPosture(void);
    
    static const int ARRAY_SIZE;

private:
    const LineMonitor     *mLineMonitor;
    BalancingWalker       *mBalancingWalker;
    ev3api::TouchSensor   &mTouchSensor;
    int   mState;
    int   mBtCommand;

    // 移動ベクタ記録用
    int   mLvec[300];
    int   mRvec[300];
    int   insertIdx;
    int   startIdx;
    int   numOfSize;

    int   mVelocity[300];
    int   insertIdxV;
    int   startIdxV;
    int   numOfSizeV;

};

#endif  /* NAVIGATOR_H */
