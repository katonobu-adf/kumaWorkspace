
#ifndef SPIN_H
#define SPIN_H

#include "TaskHolder.h"
#include "Navigator.h"
#include "LineMonitor.h"
#include "BalancingWalker.h"

class Spin : public TaskHolder{
public:
    Spin(
           Navigator * navigator,
           const LineMonitor * lineMonitor,
           BalancingWalker * balancingWalker,
           ev3api::Motor &tail);
    int run(void);

private:
    int callCount;
// 奥山追加 <end>    
};
#endif /* SPIN_H */
