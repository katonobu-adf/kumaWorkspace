
#include "Driver.h"

const int  Driver::EMERGENCY_STOP = -1;  // 緊急停止

const int  Driver::CALIBRATION    =  0;   // キャリブレーション
const int  Driver::READY_TO_START =  1;   // スタート前起立 (Start合図待ち)
const int  Driver::START_TO_DASH  =  2;   // (安定化) スタート
const int  Driver::LINE_TRACER    =  3;   // ライントレース
const int  Driver::WALK_ON_GRAY   =  4;   // 灰色マーカ上走行

Driver::Driver(
           Navigator * navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : mNavigator(navigator),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false)
{
    mNavigator->setState(CALIBRATION);
    emergencyStop = new EmergencyStop( mNavigator, mBalancingWalker, mTail );

    mTaskHolder[0] = new Calibration(  mNavigator, mBalancingWalker, mTail );
    mTaskHolder[1] = new ReadyToStart( mNavigator, mBalancingWalker, mTail );
    mTaskHolder[2] = new StartToDash(  mNavigator, mBalancingWalker, mTail );
    mTaskHolder[3] = new LineTracer(   mNavigator, mBalancingWalker, mTail );
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
            ret=mTaskHolder[0]->run();
            if ( ret == 1 ){ mNavigator->setState(READY_TO_START);}
            break;

        case READY_TO_START:  // 走行体起立、合図待ち
            ret=mTaskHolder[1]->run();
            // 安定化スタートへ
            if ( ret == 1 ){ mNavigator->setState(START_TO_DASH);}
            // ライントレースへ
            //if ( ret == 1 ){ mNavigator->setState(LINE_TRACE);}
            break;

        case START_TO_DASH:   // 安定化スタート （ちょっとの間直線走行）
            ret=mTaskHolder[2]->run();
            if ( ret == 1 ){ mNavigator->setState(LINE_TRACER);}
            break;

        case LINE_TRACER:     // ライントレースだよ
            mTaskHolder[3]->setCurDirection(curDirection);
            ret=mTaskHolder[3]->run();
            if ( ret == 1 ){ mNavigator->setState(2);}
            break;
        default:
            break;
    }
    return 0;
}