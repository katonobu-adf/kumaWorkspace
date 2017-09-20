/******************************************************************************
 *  Step.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Rena Onishi, Yukou Sugawara, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Step.h"

/* 円周率 */
#define PI 3.14159265358
#define TIRE_DIAMETER 81.0  //タイヤ直径（81mm）

/**
 * コンストラクタ
 * @param mNavigator         mNavigator
 * @param balancingWalker   倒立振子ライブラリのラッパ―
 * @param tail              尻尾のモーター
 */
Step::Step(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : LineTracer(navigator, balancingWalker, tail) {

    cyc_cnt = 0; 
    active_mode = 1; 
    mIsInitialized = false;
    forward = 22;

    distance = 0.0;     //走行距離
    distance4msL = 0.0; //左タイヤの4ms間の距離
    distance4msR = 0.0; //右タイヤの4ms間の距離
    pre_angleL = 0.0;
    pre_angleR = 0.0; // 左右モータ回転角度の過去値
    leftWheelOffset = 0;
    rightWheelOffset = 0;

    edgeChanged = false;

    stepCount = 0;
    noResponseTimeRemain = 0;

    in1stStep = false;
    in2ndStep = false;
    in2ndStepMiddle = false;
    notes[0] = NOTE_C4;
    notes[1] = NOTE_C5;
    notes[2] = NOTE_C6;
    
    mLogging = new Logging();
}

/**
 * 階段
 */
int Step::run() {
    int brightness;
    int direction = 0;    
    
    // 現在の明るさを求める
    brightness = mNavigator->getBrightness();
    // 走行体の向きを計算する（ライントレースする）
    if (stepCount == 0) {
        direction = calcDirection(brightness, forward);
    }

    if (mIsInitialized == false) {
        mIsInitialized = true;
        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
        // mBalancingWalker->setCommand(forward, direction);
        ev3_speaker_play_tone(NOTE_C4, 200);
    }

    if(active_mode == 1){
        cyc_cnt++;

        // しっぽおろす
        tail_control(80); /* ルックアップゲート角度に制御 */
        
        // 衝撃検出
        // ジャイロの他に車輪のデータも使った方が確実そう
        // 現時点では段差乗り上げ直前の衝撃を検出している感じ
        if (noResponseTimeRemain != 0) {
            --noResponseTimeRemain;
        }
        if (noResponseTimeRemain == 0) {
            if (mNavigator->shockDetectEx3(140, 100)) {
                // 150,120ではクッション素材だとNG
            // if (mNavigator->shockDetectEx3(150, 120)) {
                    // 175,120:検出漏れあり
                // 200,120:エボルタで1回ノースピン＋ガレージイン数cmアウトまで行ったが、その後検出無しが続く
                // 150,120でも2段目上った後の前後動で誤検出あり
                // 150,100は定常走行時の誤検出けっこうある
                // 不感帯セット
                // 500(2sec)でも厳しい時ある
                noResponseTimeRemain = 750;
                ++stepCount;
                mNavigator->setStepCount(stepCount);
                if (stepCount < 4) {
                    ev3_speaker_play_tone(notes[stepCount - 1], 200);
                    // ev3_speaker_play_tone(NOTE_C4, 200);
                } else {
                    ev3_speaker_play_tone(NOTE_B6, 200);
                }
            }
        }

        if (stepCount == 1) {
            // 1段目で減速
            if (in1stStep == false) {
                in1stStep = true;
                cyc_cnt = 0;
                Distance_init();                
            }
            direction = 0;
            Distance_update();
            // 昇ったらしばらくは減速、5cm走行後は加速（2段目に備える）
            // ここで一旦体制を整える動きがあってもいいかも
            if (distance <= 50) {
                forward = 15;
            } else {
                forward = 40;
            }
        }
        else if (stepCount == 2) {

            if (in2ndStep == false) {
                in2ndStep = true;
                cyc_cnt = 0;
                Distance_init();                
            }
            direction = 0;
            Distance_update();

            // 昇ったあたりで減速
            if (distance <= 50) {
                forward = 15;
            } else {
                if (distance < 75) {
                // if (distance < 100) {
                    // オーバーラン考慮して15cmより短め設定12.5でもオーバーラン気味
                    // forward値デバッグ用（本来は10）
                    forward = 12;
                } else {
                    if (in2ndStepMiddle == false) {
                        in2ndStepMiddle = true;
                        cyc_cnt = 0;
                    }
                    forward = 10;
                }
            }
            // 2段目上り終えてから左90°ターン(250カウントで90°換算だが、forward値で変わるはず)
            if (in2ndStepMiddle && (cyc_cnt > 250 && cyc_cnt < 500)) {
                direction = -30;
            }
        }
        else if (stepCount >= 3) {
            // 段差を3回検出(1,2段目昇りと2段目からの降り）
            ev3_speaker_play_tone(NOTE_C4, 200);
            return 1;
        }
        mBalancingWalker->setCommand(forward, direction);
        mBalancingWalker->run();
    }
    return 0;
}

/* 初期化関数 */
void Step::Distance_init() {
    //各変数の値の初期化
    distance = 0.0;
    distance4msR = 0.0;
    distance4msL = 0.0;
    //モータ角度の過去値に現在値を代入
    pre_angleL = mBalancingWalker->getLWheelOffset() - leftWheelOffset;
    pre_angleR = mBalancingWalker->getRWheelOffset() - rightWheelOffset;
}

/* 距離更新（4ms間の移動距離を毎回加算している） */
void Step::Distance_update(){
    float cur_angleL = mBalancingWalker->getLWheelOffset() - leftWheelOffset; //左モータ回転角度の現在値
    float cur_angleR= mBalancingWalker->getRWheelOffset() - rightWheelOffset;//右モータ回転角度の現在値
    float distance4ms = 0.0;        //4msの距離

    // 4ms間の走行距離 = ((円周率 * タイヤの直径) / 360) * (モータ角度過去値　- モータ角度現在値)
    distance4msL = ((PI * TIRE_DIAMETER) / 360.0) * (cur_angleL - pre_angleL);  // 4ms間の左モータ距離
    distance4msR = ((PI * TIRE_DIAMETER) / 360.0) * (cur_angleR - pre_angleR);  // 4ms間の右モータ距離
    distance4ms = (distance4msL + distance4msR) / 2.0; //左右タイヤの走行距離を足して割る
    distance += distance4ms;

    // mLogging->send(distance/);    

    //モータの回転角度の過去値を更新
    pre_angleL = cur_angleL;
    pre_angleR = cur_angleR;
}

/* 走行距離を取得 */
float Step::Distance_getDistance(){
    return distance;
}

/* 右タイヤの4ms間の距離を取得 */
float Step::Distance_getDistance4msRight(){
    return distance4msR;
}

/* 左タイヤの4ms間の距離を取得 */
float Step::Distance_getDistance4msLeft(){
    return distance4msL;
}

    
