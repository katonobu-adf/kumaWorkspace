
/******************************************************************************
 *  Driver.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Driver.h"

const int  Driver::EMERGENCY_STOP = -1;  // 緊急停止

const int  Driver::CALIBRATION    =  0;   // キャリブレーション
const int  Driver::READY_TO_START =  1;   // スタート前起立 (Start合図待ち)
const int  Driver::START_TO_DASH  =  2;   // (安定化) スタート
const int  Driver::LINE_TRACER    =  3;   // ライントレース
// ADF)加藤
// 灰色マーカを廃止して番号を詰めた
const int  Driver::SPEED_DOWN     =  4;   // 難所前のつなぎ
const int  Driver::LOOK_UP_GATE   =  5;   // ルックアップゲート
const int  Driver::STEP   =  6;   // 階段
const int  Driver::GARAGE_IN =  7;   // ガレージイン（Lコース）
// ADF)加藤

Driver::Driver(
           Navigator       *navigator,
           BalancingWalker *balancingWalker,
           ev3api::Motor   &tail)
    : mNavigator(navigator),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false)
{
    mNavigator->setState(CALIBRATION);
    emergencyStop = new EmergencyStop( mNavigator, mBalancingWalker, mTail );

    mTask[0] = new Calibration(  mNavigator, mBalancingWalker, mTail );
    mTask[1] = new ReadyToStart( mNavigator, mBalancingWalker, mTail );
    mTask[2] = new StartToDash(  mNavigator, mBalancingWalker, mTail );
    mTask[3] = new LineTracer(   mNavigator, mBalancingWalker, mTail );
    // ADF)加藤
    // 4は独自メソッドがある関係上欠番とし、専用変数で保持
    speedDown = new SpeedDown( mNavigator, mBalancingWalker, mTail );
    // mTask[5] = new LookUpGate(   mNavigator, mBalancingWalker, mTail );
    lookUpGate = new LookUpGate(   mNavigator, mBalancingWalker, mTail );
    mTask[6] = new Step(   mNavigator, mBalancingWalker, mTail );
    mTask[7] = new GarageIn(   mNavigator, mBalancingWalker, mTail );
    // ADF)加藤
    mLogging = new Logging();
}

// 4ms周期で呼ばれる本体
int Driver::run()
{
    int ret;
    
    // ナビに現在の状態を訊いてみる
    int state = mNavigator->getState();

    // 走行中ならば
    if ( state != CALIBRATION && state != READY_TO_START ){
        // ナビにジャイロセンサーの値を記録するように指示する
        mNavigator->recordGyro();
        // ナビに現在の本体の姿勢を訊いてみる
        float curPosture = mNavigator->getPosture();
        //mLogging->send(curPosture);
        // 姿勢がどう考えても倒れているようなら、モータを止める
        if( curPosture < -10000 || curPosture > 10000 ){
            mNavigator->setState(EMERGENCY_STOP);
        }
    }
    
    // ナビに移動方向を記録するように指示する
    mNavigator->recordVector();
    // ナビに現在の向いている方向を訊いてみる
    float curDirection = mNavigator->getVector();

    // ナビに現在の状態を訊いてみる
    state = mNavigator->getState();
    
    // 状態に適した走行をする
    // run()から 1が戻ってきたら、次の走行形式に移行
    // 緊急停止の時は Driver::run()自体が return 1; => タスク終了
    switch(state){
        case EMERGENCY_STOP: // 緊急停止！
            ev3_speaker_play_tone(NOTE_C5, 500);
            emergencyStop->stop();
            return 1; // exit to Task

        case CALIBRATION:    // キャリブレーション
            ret=mTask[0]->run();
            if ( ret == 1 ){ mNavigator->setState(READY_TO_START);}
            break;

        case READY_TO_START:  // 走行体起立、合図待ち
            ret=mTask[1]->run();
            // 安定化スタートへ
            if ( ret == 1 ){ mNavigator->setState(START_TO_DASH);}
            // ライントレースへ
            //if ( ret == 1 ){ mNavigator->setState(LINE_TRACE);}
            break;

        case START_TO_DASH:   // 安定化スタート （ちょっとの間直線走行）
            ret=mTask[2]->run();
            if ( ret == 1 ){ mNavigator->setState(LINE_TRACER);}
            break;

        case LINE_TRACER:     // ライントレースだよ
            mTask[3]->setCurDirection(curDirection);
            ret=mTask[3]->run();
            // ADF)加藤
            if ( ret == 1 ){
                // 以下は forward90 > 20 へ1000msecで移行するという意味
                // speedDown->setParameter(90, 20, 1000);
                speedDown->setParameter(90, 20, 4000);
                mNavigator->setState(SPEED_DOWN);
            }
            // ADF)加藤
            break;
        // ADF)加藤
        case SPEED_DOWN:
            ret = speedDown->run();
            if ( ret == 1 ){ mNavigator->setState(STEP);}
            break;
        case LOOK_UP_GATE:     // ルックアップゲート
            ret = lookUpGate->run();
            break;
        case STEP:     // 階段
            ret = mTask[STEP]->run();
            if ( ret == 1 ){ mNavigator->setState(GARAGE_IN);}
            break;
        case GARAGE_IN:     // ガレージイン（Lコース）
            ret = mTask[GARAGE_IN]->run();
            break;
        // ADF)加藤
        default:
            break;
    }
    return 0;
}

// ADF)加藤
int Driver::getState()
{
    return mNavigator->getState();
}
// ADF)加藤