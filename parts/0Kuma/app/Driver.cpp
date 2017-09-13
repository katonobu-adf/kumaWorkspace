
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

// 灰色マーカを廃止して番号を詰めた
const int  Driver::SPEED_DOWN     =  4;   // 難所前のつなぎ
const int  Driver::LOOK_UP_GATE   =  5;   // ルックアップゲート

const int COLLAPSE = 10000;
const int EMERGENCY_CALL_DURATION = 500;
const int SPEED_DOWN_UPPER = 90;
const int SPEED_DOWN_LOWER = 20;
const int SPEED_DOWN_DURATION = 4000;

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
    mTask[5] = new LookUpGate(   mNavigator, mBalancingWalker, mTail );
    // ADF)加藤
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
        // 姿勢がどう考えても倒れているようなら、モータを止める
        if( curPosture < -COLLAPSE || curPosture > COLLAPSE ){
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
            ev3_speaker_play_tone(NOTE_C5, EMERGENCY_CALL_DURATION);
            emergencyStop->stop();
            return 1; // exit to Task

        case CALIBRATION:    // キャリブレーション
            ret = mTask[0]->run();
            if ( ret == 1 ){ mNavigator->setState(READY_TO_START);}
            break;

        case READY_TO_START:  // 走行体起立、合図待ち
            ret = mTask[1]->run();
            // 安定化スタートへ
            if ( ret == 1 ){ mNavigator->setState(START_TO_DASH);}
            break;

        case START_TO_DASH:   // 安定化スタート （ちょっとの間直線走行）
            ret = mTask[2]->run();
            if ( ret == 1 ){ mNavigator->setState(LINE_TRACER);}
            break;

        case LINE_TRACER:     // ライントレースだよ
            mTask[3]->setCurDirection(curDirection);
            ret = mTask[3]->run();

            if ( ret == 1 ){
                // 以下は forward90 > 20 へ4000msecで移行するという意味
                speedDown->setParameter(SPEED_DOWN_UPPER, SPEED_DOWN_LOWER, SPEED_DOWN_DURATION);
                mNavigator->setState(SPEED_DOWN);
            }

            break;

        case SPEED_DOWN:
            ret = speedDown->run();
            if ( ret == 1 ){ mNavigator->setState(LOOK_UP_GATE);}
            break;
        case LOOK_UP_GATE:     // ルックアップゲート
            ret = mTask[LOOK_UP_GATE]->run();
            break;
        default:
            break;
    }
    return 0;
}

int Driver::getState()
{
    return mNavigator->getState();
}
