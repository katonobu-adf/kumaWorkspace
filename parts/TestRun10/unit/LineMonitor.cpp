
/******************************************************************************
 *  LineMonitor.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "LineMonitor.h"

// 定数宣言
const int    LineMonitor::ARRAY_SIZE = 20;         // 100を超えないこと

/**
 * コンストラクタ
 * @param colorSensor カラーセンサ
 */
LineMonitor::LineMonitor(const ev3api::ColorSensor& colorSensor, 
                         int8_t  initialThreshold )
    : mColorSensor(colorSensor),
      mThreshold(initialThreshold) {
          numOfSize=0;
          insertIdx=0;
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