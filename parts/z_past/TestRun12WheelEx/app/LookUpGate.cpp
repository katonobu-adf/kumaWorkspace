
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

const float LookUpGate::DISTANCE_TO_GARAGE = 1000.0;

// using宣言
using ev3api::Motor;
using ev3api::TouchSensor;

// オブジェクトの定義
bool mStopped = false;
bool mStanding = false;
int mode = 1;
static int cyc_cnt = -500; 
bool notYetInitialized = true;

//----------------
bool mIsInitialized = false;
int forward = 20;
static float distance = 0.0;     //走行距離
static float distance4msL = 0.0; //左タイヤの4ms間の距離
static float distance4msR = 0.0; //右タイヤの4ms間の距離
static float pre_angleL, pre_angleR; // 左右モータ回転角度の過去値
static int active_mode = 1; 

static int leftWheelOffset = 0;
static int rightWheelOffset = 0;

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
    : Task(navigator, balancingWalker, tail) {
        // mLogging = new Logging();
}

/**
 * ルックアップゲートを通過＆戻って再び通過
 */
int LookUpGate::run() {

    // 倒立走行
    if (mIsInitialized == false) {
        // mBalancingWalker->init();
        mIsInitialized = true;

        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
        mBalancingWalker->setCommand(forward, 0);
    }

    if(active_mode == 1 ){
        cyc_cnt++;

        // しっぽおろす
        tail_control(80); /* ルックアップゲート角度に制御 */
        if(mNavigator->getDistance() < 15){
            mBalancingWalker->setCommand(0, 0);
            active_mode = 2;
            Distance_init();
            cyc_cnt = 0;
        }
        mBalancingWalker->run();
    }
    
    if(active_mode == 2){
        //距離計算
        Distance_update();

        cyc_cnt++;
        if(cyc_cnt < 200){
            tail_control(75); /* ルックアップゲート角度に制御 */
            mBalancingWalker->run();
        }
        else if(cyc_cnt < 350){
            // しっぽをおろしていく
            mBalancingWalker->blindWalk(20, 20);
            tail_control(70); /* ルックアップゲート角度に制御 */
        }
        else if(cyc_cnt < 600){
            // しっぽをおろしていく
            mBalancingWalker->blindWalk(20, 20);
            tail_control(65); /* ルックアップゲート角度に制御 */
        }
        else if(cyc_cnt < 850){
            // しっぽを最大角度までおろす
            tail_control(TAIL_ANGLE_LOOK_UP); /* ルックアップゲート角度に制御 */
            mBalancingWalker->blindWalk(20, 20);
        }
        else if(cyc_cnt < 2050){
            tail_control(TAIL_ANGLE_LOOK_UP); /* ルックアップゲート角度に制御 */
            mBalancingWalker->blindWalk(-10, -10);
        }
        else if(cyc_cnt < 3050){
            //通過後の距離を算出する。（グレーラインまでの距離）そのあとで後ろの処理を計算して出す。！
            tail_control(TAIL_ANGLE_LOOK_UP); /* ルックアップゲート角度に制御 */
            mBalancingWalker->blindWalk(20, 20);
            active_mode = 3;
            ev3_speaker_play_tone(NOTE_G4, 200);
        }
    }

    if(active_mode == 3){
        //距離計算
        Distance_update();

        if(Distance_getDistance() > DISTANCE_TO_GARAGE) {
            tail_control(TAIL_ANGLE_LOOK_UP); /* ルックアップゲート角度に制御 */
            mBalancingWalker->blindWalk(0, 0);
            return 1;
        }
        else{
            tail_control(TAIL_ANGLE_LOOK_UP); /* ルックアップゲート角度に制御 */
            mBalancingWalker->blindWalk(20, 20);
        }
    }
    return 0;
}

/* 初期化関数 */
void LookUpGate::Distance_init() {
    //各変数の値の初期化
    distance = 0.0;
    distance4msR = 0.0;
    distance4msL = 0.0;
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

