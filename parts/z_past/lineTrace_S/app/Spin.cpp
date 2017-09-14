
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
        subState=0;
}

/**
 * 適当
 */
int Spin::run() {
    // 奥山
    int ret;
    int curSpeed=mBalancingWalker->brake(0);

    callCount++;
    switch(subState){
        case 0:
            if (callCount%100==0){
                if ( curSpeed > 20 ){
                    mBalancingWalker->brake(1);
                }
                else {
                    subState = 1;
                }
            }
            mBalancingWalker->run();
            break;

        case 1:
            ret = tail_control(TAIL_ANGLE_WALK_BY, 0.5);
            if ( ret == 1 ){ subState = 2;}
            mBalancingWalker->run();
            break;
        case 2:
            mBalancingWalker->setSpinMode();
            mBalancingWalker->blindWalk(0,0);
            subState = 3;
            break;
        case 3:
            ret = mBalancingWalker->spin(740);
            if ( ret == 1 ){ subState = 4;}
            break;
        case 4:
            mBalancingWalker->unsetSpinMode();
            mBalancingWalker->blindWalk(20,20);
            subState = 5;
            break;
        default:
            break;
    }

    return 0;
}

// 奥山追加 <end>