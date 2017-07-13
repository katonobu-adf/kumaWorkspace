
#include "Navigator.h"

Navigator::Navigator(const LineMonitor* lineMonitor,
           BalancingWalker* balancingWalker,
           ev3api::TouchSensor & touchSensor )
        : mLineMonitor(lineMonitor),
          mBalancingWalker(balancingWalker),
          mTouchSensor(touchSensor)
{
    mState = 0;
    mBtCommand = 0;
}

void Navigator::setState(int state){
    mState = state;
}

int  Navigator::getState()
{
    return mState;
}


void Navigator::setBtCommand(int command){
    mBtCommand = command;
}

int  Navigator::getBtCommand()
{
    return mBtCommand;
}

bool Navigator::isTouchPressed()
{
   if (mTouchSensor.isPressed() == 1 )
   {
       return true;
   }
   return false;
}

// 佐々木追加 <begin>

// app.cpp が利用するもの
bool Navigator::isOutputRequest(){
    return request;

}

char *Navigator::getMsg(){
    request = false;
    return msg;
}
// Navigator を継承したクラスが利用できるもの
void Navigator::setMsg(int val){
    sprintf(msg, "%d, ", val);
    request = true;
}

// 佐々木追加 <end>

