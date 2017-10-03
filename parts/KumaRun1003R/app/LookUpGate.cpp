
/******************************************************************************
 *  LookUpGate.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "LookUpGate.h"

#define TAIL_ANGLE_RUN_UP  58 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_LOOK_UP  63 /* ルックアップゲート完全停止時の角度[度] */

/* 円周率 */
#define PI 3.14159265358
#define TIRE_DIAMETER 81.0  //タイヤ直径（81mm）
// runningMode = 0はそのままバック
// runningMode = 1は反転して戻る

// using宣言
using ev3api::Motor;
using ev3api::TouchSensor;

// オブジェクトの定義

/**
 * コンストラクタ
 * @param mNavigator         mNavigator
 * @param balancingWalker   倒立振子ライブラリのラッパ―
 * @param tail              尻尾のモーター
 */
LookUpGate::LookUpGate(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : LineTracer(navigator, balancingWalker, tail) {

    cyc_cnt = 0; 
    active_mode = 1; 
    
    mIsInitialized = false;

    forward = 20;
    lineTraced = false;
    runningMode = 0;
    
    distance = 0.0;     //走行距離
    distance4msL = 0.0; //左タイヤの4ms間の距離
    distance4msR = 0.0; //右タイヤの4ms間の距離
    distanceL = 0.0; //左タイヤの距離
    distanceR = 0.0; //右タイヤの距離
    pre_angleL = 0.0;
    pre_angleR = 0.0; // 左右モータ回転角度の過去値
    leftWheelOffset = 0;
    rightWheelOffset = 0;

    actualDistanceInSingle = 0;


    // mLogging = new Logging();
}

void LookUpGate::setLineTraced(bool targetValue) {
    lineTraced = targetValue;
}

bool LookUpGate::getLineTraced() {
    return lineTraced;
}

void LookUpGate::setRunningMode(int targetValue) {
    runningMode = targetValue;
}

int LookUpGate::getRunningMode() {
    return runningMode;
}

/**
 * ルックアップゲートを通過＆戻って再び通過
 */
int LookUpGate::run() {

    int brightness;
    int direction = 0;    
    
    if (lineTraced) {
        // 現在の明るさを求める
        brightness = mNavigator->getBrightness();
        // 走行体の向きを計算する
        direction = calcDirection(brightness, forward);
    }

    // 倒立走行
    if (mIsInitialized == false) {
        // mBalancingWalker->init();
        mIsInitialized = true;

        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
        mBalancingWalker->setCommand(forward, direction);
        ev3_speaker_play_tone(NOTE_C4, 200);
    }

    if (active_mode == 1) {
        cyc_cnt++;
        
        // しっぽおろす
        tail_control(80);
        if (mNavigator->getDistance() < 15) {
            // ゲート検出
            mBalancingWalker->setCommand(0, 0);
            active_mode = 2;
            Distance_init();
            cyc_cnt = 0;
        } else if (lineTraced) {
            mBalancingWalker->setCommand(forward, direction);
        }
        mBalancingWalker->run();
    }
    

    if (active_mode == 2) {
        // 倒立から尻尾走行へ移行し、シングル達成まで
        // 基本時間ベースで動作させ、シングル達成判断は距離で検出

        cyc_cnt++;
        Distance_update();

        if (cyc_cnt < 200 ){
            // 倒立維持（姿勢保持のために実測で1.2cm程度前進）
            tail_control(75);
            mBalancingWalker->run();
        } else if (cyc_cnt < 350) {
            // 倒立切る（実測で8.4cm）
            mBalancingWalker->blindWalk(20, 20);
            tail_control(70);
        } else if (cyc_cnt < 600) {
            // 実測で16.5cm
            mBalancingWalker->blindWalk(20, 20);
            tail_control(65);
        } else if (cyc_cnt < 850) {
            // 実測で16.5cm
            // ここで尻尾上げ角度最大（以後、維持）
            tail_control(TAIL_ANGLE_LOOK_UP);
            mBalancingWalker->blindWalk(20, 20);
        } else if (cyc_cnt < 1150) {
            // 実測で0.5cm
            // 後退する前に小休止
            // いきなり尻尾で後退の場合、コースのたるみを生んでしまう懸念もある為
            tail_control(TAIL_ANGLE_LOOK_UP);
            mBalancingWalker->blindWalk(0, 0);
        } else {
            // 次へ
            // 距離を保持
            actualDistanceInSingle = distance;
            cyc_cnt = 0;
            Distance_init();
            active_mode = 3;
        }
    }

    if (active_mode == 3) {
        // ダブルに向けて戻る
        // ここから先、runningModeで遷移先のactive_modeが変化する

        cyc_cnt++;
        Distance_update();

        switch (runningMode) {
            case 0:
                active_mode = 4;
                cyc_cnt = 0;
                Distance_init();
                break;
            case 1:
                active_mode = 6;
                cyc_cnt = 0;
                Distance_init();
                break;
            default:
                active_mode = 4;
                cyc_cnt = 0;
                Distance_init();
                break;
        }
    }

    if (active_mode == 4) {
        // バックで戻る

        cyc_cnt++;
        Distance_update();

        tail_control(TAIL_ANGLE_LOOK_UP);
        
        mBalancingWalker->blindWalk(-10, -10);

        if (distance < -actualDistanceInSingle) {
            // バックはマイナスになるので注意
            active_mode = 5;
            cyc_cnt = 0;
            Distance_init();
        }
    }

    if (active_mode == 5) {
        // ダブル達成までの前進（runningModeに関わらず共通）

        cyc_cnt++;
        Distance_update();

        if (cyc_cnt < 125) {
            // バック後すぐ前進でズレが生じる場合があるので少し待つ
            tail_control(TAIL_ANGLE_LOOK_UP);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 375) {
            // ゆっくり発進
            tail_control(TAIL_ANGLE_LOOK_UP);
            mBalancingWalker->blindWalk(5, 5);
        } else {
            mBalancingWalker->blindWalk(20, 20);
            tail_control(TAIL_ANGLE_LOOK_UP);
            
            if (distance > actualDistanceInSingle) {
                active_mode = 9;
                cyc_cnt = 0;
                Distance_init();
            }
        }
    }

    if (active_mode == 6) {
        // 180°転回（戻る準備）

        cyc_cnt++;
        Distance_update();

        tail_control(TAIL_ANGLE_LOOK_UP);
        
        if (distanceR < 254) {
            mBalancingWalker->blindWalk(-6, 6);
        } else {
            active_mode = 7;
            cyc_cnt = 0;
            Distance_init();
        }
    }

    if (active_mode == 7) {
        // 前進して戻る

        cyc_cnt++;
        Distance_update();

        tail_control(TAIL_ANGLE_LOOK_UP);
        
        if (distance < actualDistanceInSingle) {
            mBalancingWalker->blindWalk(10, 10);
        } else {
            active_mode = 8;
            cyc_cnt = 0;
            Distance_init();
        }

    }

    if (active_mode == 8) {
        // 180°転回（進行方向に向く）
        // 単純に処理ベースならactive_mode=6と共通にしたいがｍ、
        // 遷移元のactive_modeを持たないといけない
        // 万が一の誤動作や微調整の必要性も考慮し、今回は決め打ちで個別に用意

        cyc_cnt++;
        Distance_update();

        tail_control(TAIL_ANGLE_LOOK_UP);
        
        if (distanceR < 254) {
            mBalancingWalker->blindWalk(-6, 6);
        } else {
            active_mode = 5;
            cyc_cnt = 0;
            Distance_init();
        }
    }

    if (active_mode == 9) {
        // 止めて少し待つ
        ++cyc_cnt;
        Distance_update();
        
        tail_control(TAIL_ANGLE_LOOK_UP);
        mBalancingWalker->blindWalk(0, 0);

        if (cyc_cnt > 250) {
            tail_control(TAIL_ANGLE_LOOK_UP);
            ev3_speaker_play_tone(NOTE_G4, 200);
            // ガレージインへ
            return 1;
        }
    }

    mNavigator->setActiveMode(active_mode);
    
    return 0;
}

/* 初期化関数 */
void LookUpGate::Distance_init() {
    //各変数の値の初期化
    distance = 0.0;
    distance4msR = 0.0;
    distance4msL = 0.0;
    distanceL = 0.0;
    distanceR = 0.0;
    //モータ角度の過去値に現在値を代入
    pre_angleL = mBalancingWalker->getLWheelOffset() - leftWheelOffset;
    pre_angleR = mBalancingWalker->getRWheelOffset() - rightWheelOffset;
}

/* 距離更新（4ms間の移動距離を毎回加算している） */
void LookUpGate::Distance_update(){
    float cur_angleL = mBalancingWalker->getLWheelOffset() - leftWheelOffset; //左モータ回転角度の現在値
    float cur_angleR= mBalancingWalker->getRWheelOffset() - rightWheelOffset;//右モータ回転角度の現在値
    float distance4ms = 0.0;        //4msの距離

    // 4ms間の走行距離 = ((円周率 * タイヤの直径) / 360) * (モータ角度過去値　- モータ角度現在値)
    distance4msL = ((PI * TIRE_DIAMETER) / 360.0) * (cur_angleL - pre_angleL);  // 4ms間の左モータ距離
    distance4msR = ((PI * TIRE_DIAMETER) / 360.0) * (cur_angleR - pre_angleR);  // 4ms間の右モータ距離
    distanceL += distance4msL;
    distanceR += distance4msR;
    distance4ms = (distance4msL + distance4msR) / 2.0; //左右タイヤの走行距離を足して割る
    distance += distance4ms;

    // mLogging->send(distance);    

    //モータの回転角度の過去値を更新
    pre_angleL = cur_angleL;
    pre_angleR = cur_angleR;
}

/* 走行距離を取得 */
float LookUpGate::Distance_getDistance(){
    return distance;
}

/* 右タイヤの4ms間の距離を取得 */
float LookUpGate::Distance_getDistance4msRight(){
    return distance4msR;
}

/* 左タイヤの4ms間の距離を取得 */
float LookUpGate::Distance_getDistance4msLeft(){
    return distance4msL;
}

