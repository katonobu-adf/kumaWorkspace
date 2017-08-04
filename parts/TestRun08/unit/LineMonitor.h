/******************************************************************************
 *  LineMonitor.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineMonitor
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_UNIT_LINEMONITOR_H_
#define EV3_UNIT_LINEMONITOR_H_

#include "ColorSensor.h"

// クラス定義
class LineMonitor {
public:
    static const int8_t INITIAL_THRESHOLD;

    explicit LineMonitor(const ev3api::ColorSensor& colorSensor);

    bool isOnLine() const;
    void setThreshold(int8_t threshold);

// 奥山追加 <begin>
    int8_t getThreshold() const;
    int    getBrightness();
    float  getAverageBrightness();
    int    getMaxBrightness();
    int    getMinBrightness();
    // 奥山追加 <end>

private:
    const ev3api::ColorSensor& mColorSensor;
    int8_t mThreshold;

    // 平均取得用テーブルの要素数
    static const int ARRAY_SIZE;
    // 平均取得用テーブル
    int    valTable[100]; // エントリ数 100
    int    insertIdx;
    int    numOfSize;
};

#endif  // EV3_UNIT_LINEMONITOR_H_
