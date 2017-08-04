
#include "Driver.h"

const int  Driver::EMERGENCY_STOP = -1;  // 緊急停止

const int  Driver::CALIBRATION    =  0;   // キャリブレーション
const int  Driver::READY_TO_START =  1;   // スタート前起立 (Start合図待ち)
const int  Driver::START_TO_DASH  =  2;   // (安定化) スタート
const int  Driver::LINE_TRACE     =  3;   // ライントレース
const int  Driver::WALK_ON_GRAY   =  4;   // 灰色マーカ上走行

Driver::Driver(
           Navigator * navigator,
           LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : mNavigator(navigator),
      mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mTail(tail),
      mIsInitialized(false)
{
    mNavigator->setState(CALIBRATION);
    emergencyStop = new EmergencyStop(mNavigator,mLineMonitor, mBalancingWalker, mTail);

    mTaskHolder[0] = new Calibration(mNavigator,mLineMonitor, mBalancingWalker, mTail);
    mTaskHolder[1] = new ReadyToStart( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    mTaskHolder[2] = new StartToDash( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    mTaskHolder[3] = new LineTracer( mNavigator,mLineMonitor, mBalancingWalker, mTail);
    mLogging = new Logging();
}


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
        mLogging->send(curPosture);
        // 姿勢がどう考えても倒れているようなら、モータを止める
        if( curPosture < -15000 || curPosture > 15000 ){
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
    switch(state){
        case EMERGENCY_STOP:
            emergencyStop->stop();
            return 1; // exit to Task

        case CALIBRATION:
            ret=mTaskHolder[0]->run();
            if ( ret == 1 ){ mNavigator->setState(READY_TO_START);}
            break;

        case READY_TO_START:
            ret=mTaskHolder[1]->run();
            // ライントレースへ
            if ( ret == 1 ){ mNavigator->setState(LINE_TRACE);}
            // 安定化スタートへ
            if ( ret == 1 ){ mNavigator->setState(START_TO_DASH);}
            break;

        case START_TO_DASH:
            ret=mTaskHolder[2]->run();
            if ( ret == 1 ){ mNavigator->setState(LINE_TRACE);}
            break;

        case LINE_TRACE: // LineTrace
            mTaskHolder[3]->setCurDirection(curDirection);
            ret=mTaskHolder[3]->run();
            if ( ret == 1 ){ mNavigator->setState(2);}
            break;
        default:
            break;
    }
    return 0;
}