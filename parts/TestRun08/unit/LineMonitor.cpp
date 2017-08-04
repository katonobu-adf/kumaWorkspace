/******************************************************************************
 *  LineMonitor.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineMonitor
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineMonitor.h"

// 定数宣言
const int    LineMonitor::ARRAY_SIZE = 30;         // 100を超えないこと
const int8_t LineMonitor::INITIAL_THRESHOLD = 15;  // 黒色の光センサ値

/**
 * コンストラクタ
 * @param colorSensor カラーセンサ
 */
LineMonitor::LineMonitor(const ev3api::ColorSensor& colorSensor)
    : mColorSensor(colorSensor),
      mThreshold(INITIAL_THRESHOLD) {
          numOfSize=0;
          insertIdx=0;
}

/**
 * ライン上か否かを判定する
 * @retval true  ライン上
 * @retval false ライン外
 */
bool LineMonitor::isOnLine() const {
    // 光センサからの取得値を見て
    // 黒であれば「true」を、
    // そうでなければ「false」を返す
    if (mColorSensor.getBrightness() <= mThreshold) {
        return true;
    } else {
        return false;
    }
}

/**
 * ライン閾値を設定する
 * @param threshold ライン閾値
 */
void LineMonitor::setThreshold(int8_t threshold) {
    mThreshold = threshold;
}

// 奥山追加 <begin>
int8_t LineMonitor::getThreshold() const{
    return mThreshold;
}

int LineMonitor::getBrightness() {
    int val = mColorSensor.getBrightness();

    // 平均値を求めるために格納しておく
    valTable[ insertIdx % ARRAY_SIZE ] = val;
    insertIdx = (insertIdx+1) % ARRAY_SIZE;
    if( numOfSize < ARRAY_SIZE ) numOfSize++;

    return val;
}

float LineMonitor::getAverageBrightness() 
{
    float avg;
    float total = 0.0;

    // 平均を求めて返す
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSize ) break;
        total += valTable[i];
    }
    avg = total / numOfSize;

    return avg;
}

int    LineMonitor::getMaxBrightness(){
    int max=-1;
    // 最大値を求めて返す
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSize ) break;
        if ( valTable[i] > max ) max = valTable[i];
    }
    return max;
}
int    LineMonitor::getMinBrightness(){
    int min=10000;
    // 最大値を求めて返す
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSize ) break;
        if ( valTable[i] < min ) min = valTable[i];
    }
    return min;
}
// 奥山追加 <end>