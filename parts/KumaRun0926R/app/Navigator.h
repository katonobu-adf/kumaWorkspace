
/******************************************************************************
 *  Navigator.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

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
    
    // モータの回転角度の取得
    int32_t  getRWheelCount(void);
    int32_t  getLWheelCount(void);

    // ジャイロセンサー(角速度の累積）記録と集計   
    void   recordGyro(void);
    float  getPosture(void);

    // LineMonitorのラッパ
    // しきい値の設定と取得

    // static const int8_t INITIAL_THRESHOLD;
    // void   setThreshold(int8_t threshold);
    // int8_t getThreshold() const;



// 佐々木追加<begin>
    static const int8_t initialThreshold = 0;
    void setThreshold(int8_t threshold);
    int8_t getThreshold();

// 佐々木追加 <end>



    // 照度センサ値の取得
    int    getBrightness();
    float  getAverageBrightness();
    int    getMaxBrightness();
    int    getMinBrightness();
    int    getAmbient();

    // 超音波センサーの値を取得
    int    getDistance(void);

    // センサー値保存用バッファのサイズ
    static const int ARRAY_SIZE;



// 佐々木追加 <begin>
    int  white;
    void setWhite(int white);
    int  getWhite();

    int  black;
    void setBlack(int black);
    int  getBlack();

    int gray;
    void setGray(int gray);
    int getGray();
    
    int calibrate(int brightness);

// 佐々木追加 <end>

    // ADF)加藤
    int getVelocityMax();
    int getVelocityMin();
    int getVelocityRange();
    int getVelocityRangeMax();
    int getVelocityRangeMin();
    bool shockDetectEx(int rangeThreshold, int inHardActionCountThreshold);
    bool shockDetectEx2(int rangeThreshold, int inHardActionCountThreshold);
    int getActiveMode();
    void setActiveMode(int activeMode);
    int getGrayScale();
    // ADF)加藤

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

    // ADF)加藤
    int mVelocityMax;
    int mVelocityMin;
    int mVelocityRange;
    int mVelocityRanges[300];
    int insertIdxVr;
    int startIdxVr;
    int numOfSizeVr;
    int mVelocityRangeMax;
    int mVelocityRangeMin;
    int inHardActionCount;
    int prevAvgPosture;
    int activeMode;
    // ADF)加藤

};

#endif  /* NAVIGATOR_H */
