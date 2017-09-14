
/******************************************************************************
 *  Navigator.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Navigator.h"

const int    Navigator::ARRAY_SIZE = 100;        // 300を超えないこと
// const int8_t Navigator::INITIAL_THRESHOLD = 15;  // 黒色の光センサ値

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

    // ADF)加藤
    mVelocityMin = 1000;
    mVelocityMax = -1000;
    mVelocityRange = 0;
    prevAvgPosture = 0;
    inHardActionCount = 0;
    insertIdxVr = 0;
    startIdxVr = 0;
    numOfSizeVr = 0;
    mVelocityRangeMax = 0;
    mVelocityRangeMin = 0;
    inHardActionCount = 0;
    prevAvgPosture = 0;
    // ADF)加藤
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

// モータ(Right Wheel)の回転角度の取得
int32_t  Navigator::getRWheelCount(void){
    return mBalancingWalker->getRWheelOffset();
}

// モータ(Left Wheel)の回転角度の取得
int32_t  Navigator::getLWheelCount(void){
    return mBalancingWalker->getLWheelOffset();    
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
    float velocityInt = 0.0;

    // velocityの合計を求める
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSizeV ) break;
        velocityInt += mVelocity[i];
    }

    return velocityInt;
}

// /**
//  * ライン閾値を設定する
//  * @param threshold ライン閾値
//  */
// void Navigator::setThreshold(int8_t threshold) {
//     mLineMonitor->setThreshold(threshold);
// }

// // LineMonitorのラッパ
// int8_t Navigator::getThreshold() const{
//     return  mLineMonitor->getThreshold();
// }

void Navigator::setThreshold(int8_t threshold){
    mLineMonitor->setThreshold(threshold);
}

int8_t Navigator::getThreshold(){
    return mLineMonitor->getThreshold();
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

int    Navigator::getAmbient(){
    return mLineMonitor->getAmbient();
}

int    Navigator::getDistance(void)
{
    // ADF))加藤
    // bool sense = mSonarSensor.listen();
    // // ソナー感しなかったら -1;
    // if ( sense != true ){
    //     return -1;
    // }
    // ADF))加藤
    int   distanceCm = mSonarSensor.getDistance();
    return distanceCm;
}

void Navigator::setWhite(int white){
    Navigator::white = white;
}

int Navigator::getWhite(){
    return white;
}

void Navigator::setBlack(int black){
    Navigator::black = black;
}

int Navigator::getBlack(){
    return black;
}

void Navigator::setGray(int gray){
    Navigator::gray = gray;
}

int Navigator::getGray(){
    return gray;
}

int Navigator::calibrate(int brightness){
    int calibratedValue;
    int child  = brightness - getWhite(); // 分子
    int mother = getBlack() - getWhite(); // 分母

    calibratedValue = 100 - ((float)child / mother) * 100;

    if(calibratedValue > 100){
        calibratedValue = 100;

    }else if(calibratedValue < 0){
        calibratedValue = 0;

    }

    return calibratedValue;
}


// ADF))加藤
int Navigator::getVelocityMax() {
    mVelocityMax = -1000;

    for (int i=0; i<ARRAY_SIZE; i++) {
        if (i >= numOfSizeV) {
            break;
        }
        if (mVelocity[i] > mVelocityMax) {
            mVelocityMax = mVelocity[i];
        }
    }
    return mVelocityMax;
}

int Navigator::getVelocityMin() {
    mVelocityMin = 1000;
    
    for (int i=0; i<ARRAY_SIZE; i++) {
        if (i >= numOfSizeV) {
            break;
        }
        if (mVelocity[i] < mVelocityMin) {
            mVelocityMin = mVelocity[i];
        }
    }
    return mVelocityMin;
}

int Navigator::getVelocityRange() {
    // 過去100回分のデータのmax/minの差分を取得
    // 過去100回分の差分も保存
    int degSpeedMax = getVelocityMax();
    int degSpeedMin = getVelocityMin();
    int mVelocityRange = degSpeedMax - degSpeedMin;
    mVelocityRanges[ insertIdxVr % ARRAY_SIZE ] = mVelocityRange;
    insertIdxVr = (insertIdxVr + 1) % ARRAY_SIZE;
    if( numOfSizeVr < ARRAY_SIZE ) numOfSizeVr++;

    return mVelocityRange;
}

int Navigator::getVelocityRangeMax() {
    mVelocityRangeMax = -1000;

    for (int i=0; i<ARRAY_SIZE; i++) {
        if (i >= numOfSizeVr) {
            break;
        }
        if (mVelocityRanges[i] > mVelocityRangeMax) {
            mVelocityRangeMax = mVelocityRanges[i];
        }
    }
    return mVelocityRangeMax;
}

int Navigator::getVelocityRangeMin() {
    mVelocityRangeMin = 1000;
    
    for (int i=0; i<ARRAY_SIZE; i++) {
        if (i >= numOfSizeVr) {
            break;
        }
        if (mVelocityRanges[i] < mVelocityRangeMin) {
            mVelocityRangeMin = mVelocityRanges[i];
        }
    }
    return mVelocityRangeMin;
}

// 衝撃検出（立ち上がり検出付き）
bool Navigator::shockDetectEx(int rangeThreshold, int inHardActionCountThreshold) {
    
    // 過去100回分のデータのmax/minの差分を取得
    int degSpeedMax = getVelocityMax();
    int degSpeedMin = getVelocityMin();
    int range = degSpeedMax - degSpeedMin;

    // 差分値が閾値以上ならinHardActionCountをインクリメント
    // 閾値以下ならinHardActionCountを0クリア
    if (range > rangeThreshold) {
        ++inHardActionCount;
    } else {
        inHardActionCount = 0;
    }

    // inHardActionCountが閾値以上のカウントなら段差とみなす
    // 過去100回分のrangeの最小値との差が30以上
    // if (inHardActionCount > inHardActionCountThreshold &&
    //     (range - getVelocityRangeMin()) > 30) {
    if (inHardActionCount > inHardActionCountThreshold) {
        return true;
    }
    return false;
}

// 衝撃検出（別アルゴリズム）
bool Navigator::shockDetectEx2(int rangeThreshold, int inHardActionCountThreshold) {
    
    int degSpeedAvg = getPosture() / 100;

    // 指定した閾値より大きく、前回よりも増加している
    if (degSpeedAvg > prevAvgPosture && degSpeedAvg > rangeThreshold) {
        ++inHardActionCount;
    } else {
        inHardActionCount = 0;
    }

    prevAvgPosture = degSpeedAvg;

    // inHardActionCountが閾値以上のカウントなら段差とみなす
    if (inHardActionCount > inHardActionCountThreshold) {
        return true;
    }
    return false;
}
// ADF)加藤
