
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "TouchSensor.h"
#include "SonarSensor.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class Navigator {
public:
    Navigator( LineMonitor          *lineMonitor,
               BalancingWalker      *balancingWalker,
               ev3api::TouchSensor  &touchSensor,
               ev3api::SonarSensor  &sonarSensor );

    // Bluetooth用インターフェース
    void   setBtCommand(int command);
    int    getBtCommand(void);
    // タッチセンサー用インターフェース
    bool   isTouchPressed(void);

    // ドライブ状態の設定と取得
    void   setState(int state);
    int    getState(void);

    // 移動方向(車輪の回転角度の累積）記録と集計
    void   recordVector(void);
    float  getVector(void);
    
    // ジャイロセンサー(角速度の累積）記録と集計   
    void   recordGyro(void);
    float  getPosture(void);

    // LineMonitorのラッパ
    // しきい値の設定と取得
    static const int8_t INITIAL_THRESHOLD;
    void   setThreshold(int8_t threshold);
    int8_t getThreshold() const;

    // 照度センサ値の取得
    int    getBrightness();
    float  getAverageBrightness();
    int    getMaxBrightness();
    int    getMinBrightness();

    // 超音波センサーの値を取得
    int    getDistance(void);

    // センサー値保存用バッファのサイズ
    static const int ARRAY_SIZE;


private:
    LineMonitor           *mLineMonitor;
    BalancingWalker       *mBalancingWalker;
    ev3api::TouchSensor   &mTouchSensor;
    ev3api::SonarSensor   &mSonarSensor;

    int   mState;
    int   mBtCommand;

    // 移動ベクタ記録用
    int   mLvec[300];
    int   mRvec[300];
    int   insertIdx;
    int   startIdx;
    int   numOfSize;

    // ジャイロセンサー(角速度記録用)
    int   mVelocity[300];
    int   insertIdxV;
    int   startIdxV;
    int   numOfSizeV;
};

#endif  /* NAVIGATOR_H */
