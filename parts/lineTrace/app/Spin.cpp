
#include "Spin.h"

// 奥山追加 <end>
/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param balancingWalker 倒立走行
 */
Spin::Spin(
           Navigator * navigator,
           const LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail)
    : TaskHolder(navigator,lineMonitor, balancingWalker,tail) {
        callCount=0;
          ;
}

/**
 * 適当ｓ
 */
int Spin::run() {
    // 奥山
    // 10秒ライントレースしたら別のことやれ
    callCount++;
    if (callCount > 2500) {
        return 1;
    }

    // 奥山
    tail_control(TAIL_ANGLE_STAND_UP);
    //int  brightness = mLineMonitor->getBrightness();

    // 走行体の向きを計算する
    //int direction = calcDirection(brightness);

    mBalancingWalker->setCommand(-20, 0);

    // 倒立走行を行う
    mBalancingWalker->run();

    return 0;
}



// 奥山追加 <end>