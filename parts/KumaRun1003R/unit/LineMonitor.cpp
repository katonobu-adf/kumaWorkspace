
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
        // ADF)加藤
        numOfSizeRGB = 0;
        insertIdxRGB = 0;
        // ADF)加藤
          
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

// 佐々木追加 <begin>
int     LineMonitor::getAmbient(){
    int val = mColorSensor.getAmbient();
    return val;
}

// 佐々木追加 <end>


// ADF)加藤

// 
int LineMonitor::getGrayScale(bool countAverage) {
    rgb_raw_t rgb;
    mColorSensor.getRawColor(rgb);

    uint16_t r = rgb.r;
    uint16_t g = rgb.g;
    uint16_t b = rgb.b;
    
    int grayScaleValue = calcGrayScale(r, g, b);

    if (countAverage) {
        // 平均値を求めるために格納しておく
        valTableRGB[insertIdx % ARRAY_SIZE] = grayScaleValue;
        insertIdxRGB = (insertIdxRGB + 1) % ARRAY_SIZE;
        if (numOfSizeRGB < ARRAY_SIZE) {
            numOfSizeRGB++;
        }
    }

    return grayScaleValue;
}

int LineMonitor::getAverageGrayScale() 
{
    int avg;
    int total = 0.0;

    // 平均を求めて返す
    for( int i=0; i<ARRAY_SIZE; i++){
        if( i >= numOfSizeRGB ) break;
        total += valTableRGB[i];
    }
    avg = total / numOfSize;

    return avg;
}

int LineMonitor::calcGrayScale(uint16_t r, uint16_t g, uint16_t b) {

    // 人間が見るわけではないので、単純な平均を取る
    // 他にはrgb毎に係数かける、中間値、中央値、最大最小の差分など色々あり
    int result;

    // 普通の平均
    // result = (r + g + b) / 3;
    // Gだけ除外するとどうなるか緑地でも他の色が反応するのでNG
    // result = (r + b) / 2;
    // NTSC係数による加重平均（簡易版）
    result = ((2 * r) + (4 * g) + b) / 7;
    // NTSC係数による加重平均（簡易版）の緑無し
    // result = ((2 * r) + b) / 7;

    return result;
}

// ADF)加藤
