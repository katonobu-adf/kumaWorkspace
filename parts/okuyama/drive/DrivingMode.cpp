
#include "DrivingMode.h"

DrivingMode::DrivingMode()
    :mSubState(0),
     mCommand(0)        {
            ;
}
int DrivingMode::prepare() {
    return 0;
}
int DrivingMode::setCommand(int command){
    mCommand = command;
    return 0;
}
int DrivingMode::exec(){
    return 0;
}
