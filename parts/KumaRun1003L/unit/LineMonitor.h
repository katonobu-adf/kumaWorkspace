
/******************************************************************************
 *  LineMonitor.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef LINEMONITOR_H_
#define LINEMONITOR_H_

#include "ColorSensor.h"

// クラス定義
class LineMonitor {
public:
    explicit LineMonitor(const ev3api::ColorSensor& colorSensor,
                         int8_t  intialThreshold );

    void   setThreshold(int8_t threshold);
    int8_t getThreshold() const;
    int    getBrightness();
    float  getAverageBrightness();
    int    getMaxBrightness();
    int    getMinBrightness();
    int    getAmbient();

    // ADF)加藤
    int getGrayScale(bool countAverage);
    int getAverageGrayScale();
    // ADF)加藤

// ADF)加藤
private:
    int calcGrayScale(uint16_t r, uint16_t g, uint16_t b);

// ADF)加藤


private:
    const ev3api::ColorSensor& mColorSensor;
    int8_t mThreshold;

    // 平均取得用テーブルの要素数
    static const int ARRAY_SIZE;
    // 平均取得用テーブル
    int    valTable[100]; // エントリ数 100
    int    insertIdx;
    int    numOfSize;


    // ADF)加藤
    int    valTableRGB[100]; // エントリ数 100
    int    insertIdxRGB;
    int    numOfSizeRGB;
    // ADF)加藤


};

#endif  // LINEMONITOR_H_
