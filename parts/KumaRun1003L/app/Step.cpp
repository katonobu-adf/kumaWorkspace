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
    distanceL = 0.0; //左タイヤの距離
    distanceR = 0.0; //右タイヤの距離
    pre_angleL = 0.0;
    pre_angleR = 0.0; // 左右モータ回転角度の過去値
    leftWheelOffset = 0;
    rightWheelOffset = 0;

    edgeChanged = false;

    stepCount = 0;
    noResponseTimeRemain = 0;
    enableShockDetect = true;

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

    int direction = 0;    

    // しっぽは上げたまま
    tail_control(TAIL_ANGLE_DRIVE);
        
    if (mIsInitialized == false) {
        mIsInitialized = true;
        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
        // mBalancingWalker->setCommand(forward, direction);
        ev3_speaker_play_tone(NOTE_C4, 200);
    }

    // 衝撃検出
    // 段差乗り上げ直前の衝撃を検出している感じ
    if (noResponseTimeRemain != 0) {
        --noResponseTimeRemain;
    }
    if (enableShockDetect && noResponseTimeRemain == 0) {
        if (mNavigator->shockDetectEx3(140, 85)) {
        // if (mNavigator->shockDetectEx3(140, 85)) {
                // 140,100は机上＋充電池では検出性良好だが、エボルタだと不検出多い（Max値が100をあまり超えない）
            // 150,120ではクッション素材だとNG
            // 175,120:検出漏れあり
            // 200,120:エボルタで1回ノースピン＋ガレージイン数cmアウトまで行ったが、その後検出無しが続く
            // 150,120でも2段目上った後の前後動で誤検出あり
            // 150,100は定常走行時の誤検出けっこうある
            // 不感帯セット（enableShockDetectの方が優先）
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

    if(active_mode == 1){
        // 1段目の段差手前から1段目検出まで

        int brightness;
        
        // 現在の明るさを求める
        brightness = mNavigator->getBrightness();
        direction = calcDirection(brightness, forward);
        
        cyc_cnt++;
        Distance_update();
        
        if (stepCount == 1) {
            cyc_cnt = 0;
            Distance_init();                
            active_mode = 2;
        }
    }
    
    if (active_mode == 2) {
        // 1段目に上る

        cyc_cnt++;
        Distance_update();
        
        // ライントレースはOff
        // 上がるのは倒立走行任せで特に何もしない
        forward = 20;
        direction = 0;

        // 1段目上がれた＆2段目に近づくまでは衝撃検出をオフにする
        enableShockDetect = false;
        
        if (distance > 280) {
            // 2段目手前
            cyc_cnt = 0;
            Distance_init();                
            enableShockDetect = true;
            noResponseTimeRemain = 0;
            active_mode = 3;
        }
    }
    
    if (active_mode == 3) {
        // 2段目手前から2段目検出まで

        cyc_cnt++;
        Distance_update();

        forward = 17;
        direction = 0;
        
        if (stepCount == 2) {
            cyc_cnt = 0;
            Distance_init();                
            // 2段目にあがって左90度ターンまでは検出無し
            enableShockDetect = false;
            active_mode = 4;
        }
    }
    
    if (active_mode == 4) {
        // 2段目にあがる

        cyc_cnt++;
        Distance_update();

        direction = 0;

        enableShockDetect = false;
        
        if (distance < 50) {
            forward = 20;
        } else if (distance < 75) {
            // 左90度ターンに合わせて減速
            forward = 10;
        } else if (distance < 100) {
            // 左90度ターンに合わせて減速
            forward = 5;
        } else {
            // 2段目のぼって中央へ到達
            forward = 0;
            cyc_cnt = 0;
            Distance_init();                
            active_mode = 5;
        }
    }
    
    if (active_mode == 5) {
        // 左90度ターン

        cyc_cnt++;
        Distance_update();

        forward = 0;
        direction = 0;
        
        // if (distanceR < 256) {
        // 倒立走行としっぽでは距離が異なる
        // if (cyc_cnt < 195) {
        if (cyc_cnt < 250) {
            direction = -30;
        } else {
            cyc_cnt = 0;
            Distance_init();
            // 不感時間はここでリセットしておく
            noResponseTimeRemain = 0;
            active_mode = 6;
        }            
    }
    
    if (active_mode == 6) {
        // 階段降りる
        cyc_cnt++;
        Distance_update();

        forward = 12;
        direction = 0;

        // if (distance > 25) {
        if (cyc_cnt > 375) {
                // 衝撃検出オン
            enableShockDetect = true;
        }
        
        if (stepCount >= 3) {
            // 段差を3回検出(1,2段目昇りと2段目からの降り）
            ev3_speaker_play_tone(NOTE_C4, 200);
            return 1;
        }
    }

    mNavigator->setActiveMode(active_mode);
    
    mBalancingWalker->setCommand(forward, direction);
    mBalancingWalker->run();
    return 0;
}

/* 初期化関数 */
void Step::Distance_init() {
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
void Step::Distance_update(){
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

/* 任意地点から開始 */
void Step::setActiveMode(int activeMode){
    active_mode = activeMode;
}
    
/* 任意地点から開始 */
void Step::setStepCount(int stepCount){
    Step::stepCount = stepCount;
}
