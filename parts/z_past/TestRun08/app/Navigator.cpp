
#include "Navigator.h"

const int    Navigator::ARRAY_SIZE = 100;        // 300を超えないこと
const int8_t Navigator::INITIAL_THRESHOLD = 15;  // 黒色の光センサ値

Navigator::Navigator( LineMonitor         *lineMonitor,
                      BalancingWalker     *balancingWalker,
                      ev3api::TouchSensor &touchSensor,
                      ev3api::SonarSensor &sonarSensor )
        : mLineMonitor(lineMonitor),
          mBalancingWalker(balancingWalker),
          mTouchSensor(touchSensor),
          mSonarSensor(sonarSensor)
{
    mState     = 0;
    mBtCommand = 0;

    startIdx   = 0;
    insertIdx  = 0;
    numOfSize  = 0;
    startIdxV   = 0;
    insertIdxV  = 0;
    numOfSizeV  = 0;
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
   if ( mTouchSensor.isPressed() == 1 ){
       return true;
   }
   return false;
}

// 移動方向(ベクタ)の記録
void Navigator::recordVector(void)
{
    int lvec, rvec;
    
    // 車輪の回転数(度)を取得
    lvec = mBalancingWalker->getLWheelOffset();
    rvec = mBalancingWalker->getRWheelOffset();

    mLvec[ insertIdx % ARRAY_SIZE ] = lvec;
    mRvec[ insertIdx % ARRAY_SIZE ] = rvec;
    insertIdx = (insertIdx+1) % ARRAY_SIZE;
    if( numOfSize < ARRAY_SIZE ) numOfSize++;
}

// 移動方向(ベクタ)の平均の取得
float  Navigator::getVector(void){
    
    int def=0;
    float avg;

    // 差分の平均を求める
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSize ) break;
        def += (mLvec[i]-mRvec[i]);
    }

    avg = (float)def/numOfSize/360.0;
    return avg;
}
void   Navigator::recordGyro(void){
    int velocity;
    velocity = mBalancingWalker->getSavedVelocity();
    mVelocity[ insertIdxV % ARRAY_SIZE ] = velocity;
    insertIdxV = (insertIdxV+1) % ARRAY_SIZE;
    if( numOfSizeV < ARRAY_SIZE ) numOfSizeV++;
}

float  Navigator::getPosture(void){
    float velocityInt=0.0;

    // velocityの合計を求める
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSizeV ) break;
        velocityInt += mVelocity[i];
    }

    return velocityInt;
}

/**
 * ライン閾値を設定する
 * @param threshold ライン閾値
 */
void Navigator::setThreshold(int8_t threshold) {
    mLineMonitor->setThreshold(threshold);
}

// LineMonitorのラッパ
int8_t Navigator::getThreshold() const{
    return  mLineMonitor->getThreshold();
}

int Navigator::getBrightness() {
    return mLineMonitor->getBrightness();
}

float Navigator::getAverageBrightness() 
{
    return mLineMonitor->getAverageBrightness();
}

int    Navigator::getMaxBrightness(){
    return mLineMonitor->getMaxBrightness();
}

int    Navigator::getMinBrightness(){
    return mLineMonitor->getMinBrightness();
}

int    Navigator::getDistance(void)
{
    bool sense = mSonarSensor.listen();
    // ソナー感しなかったら -1;
    if ( sense != true ){
        return -1;
    }
    int   distanceCm = mSonarSensor.getDistance();
    return distanceCm;
}