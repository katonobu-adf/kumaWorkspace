/******************************************************************************
 *  GarageInR.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/10
 *  Author: Takuma Sato, Nobuhiko Kato
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "GarageInR.h"

#define TAIL_ANGLE_RUN_UP  58 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_LOOK_UP  63 /* ルックアップゲート完全停止時の角度[度] */
#define TAIL_ANGLE_DETECT_LINE 80 /* ライン検出時角度[度] */

/* 円周率 */
#define PI 3.14159265358
#define TIRE_DIAMETER 81.0  //タイヤ直径（81mm）
// ガレージまでの総走行距離（mm）
#define DISTANCE_TO_GARAGE_RUNNING_MODE0 300.0

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
GarageInR::GarageInR(
           Navigator* navigator,
           BalancingWalker* balancingWalker,
           ev3api::Motor &tail)
    : LineTracer(navigator, balancingWalker, tail) {

    cyc_cnt = 0; 
    active_mode = 1; 
    
    mIsInitialized = false;

    forward = 20;
    
    distance = 0.0;     //走行距離
    distance4msL = 0.0; //左タイヤの4ms間の距離
    distance4msR = 0.0; //右タイヤの4ms間の距離
    distanceL = 0.0; //左タイヤの距離
    distanceR = 0.0; //右タイヤの距離
    pre_angleL = 0.0;
    pre_angleR = 0.0; // 左右モータ回転角度の過去値
    leftWheelOffset = 0;
    rightWheelOffset = 0;

    onGroundMax.time = 0;
    onGroundMax.value = 0;
    onGroundMax.distance = 0;
    onGroundMin.time = 0;
    onGroundMin.value = 1000;
    onGroundMin.distance = 0;
    
    groundEnvironment = 0;
    turnForRecoveryTime = 0;
    distanceToGarage = 550;

    notWhiteAreaDetected = false;
    notYetDetectBlack = true;

    // mLogging = new Logging();
}

/**
 * ルックアップゲート後のライン復帰とガレージイン（Rコース専用）
 */
int GarageInR::run() {

    if (mIsInitialized == false) {
        mIsInitialized = true;
        leftWheelOffset = mBalancingWalker->getLWheelOffset();
        rightWheelOffset = mBalancingWalker->getRWheelOffset();
        ev3_speaker_play_tone(NOTE_C4, 200);
        Distance_init();
    }

    if(active_mode == 1){
        ++cyc_cnt;

        // 少しずつ尻尾を下す
        // 前のフェーズで63°（荷重がかかるので場合によっては60°くらい）
        // ここから徐々に（5°ずつくらい）80°までおろしていく

        // 原理は後進時の車輪の反作用を利用してつっかえ棒をちょっとずつ立てていくイメージ

        if (cyc_cnt < 100) {
            tail_control(62);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 150) {
            tail_control(65);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 200) {
            tail_control(65);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 250) {
            tail_control(65);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 300) {
            tail_control(67);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 350) {
            tail_control(67);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 400) {
            tail_control(67);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 450) {
            tail_control(69);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 500) {
            tail_control(69);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 550) {
            tail_control(69);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 600) {
            tail_control(71);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 650) {
            tail_control(71);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 700) {
            tail_control(71);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 750) {
            tail_control(73);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 800) {
            tail_control(73);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 850) {
            tail_control(73);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 900) {
            tail_control(75);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 950) {
            tail_control(75);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 1000) {
            tail_control(75);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 1050) {
            tail_control(77);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 1100) {
            tail_control(77);
            mBalancingWalker->blindWalk(0, 0);
        } else if (cyc_cnt < 1150) {
            tail_control(77);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 1200) {
            tail_control(79);
            mBalancingWalker->blindWalk(-3, -3);
        } else if (cyc_cnt < 1250) {
            tail_control(79);
            mBalancingWalker->blindWalk(0, 0);
        } else {
            tail_control(79);
            mBalancingWalker->blindWalk(0, 0);
        }

        if (cyc_cnt > 1500) {
            // ルックアップゲートからの連続動作だと誤検出のおそれがあるので、時間を空ける
            active_mode = 2;
            Distance_init();
            cyc_cnt = 0;
        }
    }
    
    if(active_mode == 2){
        // 左スピンで走査

        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        mBalancingWalker->blindWalk(-6, 6);
        int grayScaleValue = mNavigator->getGrayScale();

        // グレー値の最大最小取得
        if (onGroundMax.value < grayScaleValue) {
            onGroundMax.time = cyc_cnt;
            onGroundMax.value = grayScaleValue;
            onGroundMax.distance = distanceR;
        }

        if (notYetDetectBlack && onGroundMin.value > grayScaleValue) {
            if (grayScaleValue < 15) {
                // 黒と思われる領域を見つけたら以後は更新しない
                // 2回目の検出で左右逆に検出されるケースがあったため
                notYetDetectBlack = false;
            }
            onGroundMin.time = cyc_cnt;
            onGroundMin.value = grayScaleValue;
            onGroundMin.distance = distanceR;
        }

        if (distanceR > 509) {
            // 1周（254mm x 2回転分で1周）したら
            active_mode = 3;
            cyc_cnt = 0;
            Distance_init();
            // 何かの都合で1周検出できないと、永遠にここから抜け出ないが、
            // その可能性は低い（スリップしても距離自体はカウントされる）
        }
    }

    if (active_mode == 3) {
        // 走査結果でラインを探索

        ++cyc_cnt;
        tail_control(TAIL_ANGLE_DETECT_LINE);
        Distance_update();
        
        groundEnvironment = judgeEnvironment(onGroundMax, onGroundMin);
        int grayScaleValue = 0;
        int minValueOffset = 5;
        
        switch (groundEnvironment) {
            case 0:
                // 中央近辺
                // 中央より進行方向右よりだと、ここに来る
                // 中央より進行方向左よりだと、ここには来ず、左側で判定される（はず）
                // この場合は、そのまま右ライントレースしていい
                cyc_cnt = 0;
                active_mode = 6;
                Distance_init();
                break;
            case 1:
                // 右側
                // 左ターンさせて黒ラインを探す
                mBalancingWalker->blindWalk(-6, 6);
                
                grayScaleValue = mNavigator->getGrayScale();

                if (grayScaleValue < (onGroundMin.value + minValueOffset)) {
                    // 黒ライン見つかった
                    mBalancingWalker->blindWalk(0, 0);
                    active_mode = 4;
                    turnForRecoveryTime = cyc_cnt;
                    cyc_cnt = 0;
                    Distance_init();
                }
                break;
            case 2:
                // 左側
                // 右ターンさせて黒ラインを探す
                mBalancingWalker->blindWalk(6, -6);

                grayScaleValue = mNavigator->getGrayScale();
                
                if (grayScaleValue < (onGroundMin.value + minValueOffset)) {
                    // 黒ライン見つかった
                    mBalancingWalker->blindWalk(0, 0);
                    active_mode = 4;
                    turnForRecoveryTime = cyc_cnt;
                    cyc_cnt = 0;
                    Distance_init();
                }
                break;
            case 3:
            default:
                // 左右どちらかに大幅にズレている
                // 横断モード発動
                if (distanceR < 127) {
                    // 左90度スピン
                    mBalancingWalker->blindWalk(-6, 6);
                } else {
                    // 黒ラインと思われるところまで前進
                    mBalancingWalker->blindWalk(8, 8);
                    grayScaleValue = mNavigator->getGrayScale();

                    // ここで緑地の検出もしたい
                    // 特徴としては黒ラインなら通過すると数値増える
                    // 緑地なら同じ値が続く
                    // 多少無駄に走らせないと検出できない
                    // 10あたりまでつっこませるか

                    if ((onGroundMax.value - grayScaleValue) > 40 && notWhiteAreaDetected == false) {
                        // 緑でもここまで来ることある
                        // 緑か黒かフラグを立てて、直進継続させて、
                        // cyc_cntで125(0.5sec)まで見る
                        notWhiteAreaDetected = true;
                        cyc_cnt = 0;
                    }
                    
                    if (notWhiteAreaDetected) {
                        if (grayScaleValue < 15) {
                            // 黒ライン確定
                            active_mode = 7;
                            cyc_cnt = 0;
                            Distance_init();
                        } else if(cyc_cnt > 125) {
                            // 緑だと思う
                            active_mode = 8;
                            cyc_cnt = 0;
                            Distance_init();
                        }
                    }
                }
                break;
        }

    }
        
    if (active_mode == 4) {
        // 走行体の中心をセンサー計測光照射位置まで持って行く
        // しっぽ角度80°で計算して、46mm前進でほぼ中央
        ++cyc_cnt;
        tail_control(TAIL_ANGLE_DETECT_LINE);
        Distance_update();

        mBalancingWalker->blindWalk(6, 6);
        
        if (distance > 46) {
            active_mode = 5;
            cyc_cnt = 0;
            Distance_init();
        }
    }

    if (active_mode == 5) {
        // 走行体中心をライン上に持ってきてからのターン
        ++cyc_cnt;
        tail_control(TAIL_ANGLE_DETECT_LINE);
        
        int grayScaleValue = mNavigator->getGrayScale();
        
        int threshold = (onGroundMax.value + onGroundMin.value) / 2;

        // 開始位置からラインまでの移動時間を使うと、
        // 開始位置でのズレまで再現していまうので、閾値に変更
        // if (cyc_cnt < turnForRecoveryTime) {
        // 理論通りならセンサーは白地を照射している
        if (grayScaleValue > threshold) {
            switch (groundEnvironment) {
                case 0:
                    // 中央近辺
                    break;
                case 1:
                    // 右側
                    // active_mode=4でラインを跨いで左側にいるので、右ターン
                    mBalancingWalker->blindWalk(6, -6);
                    break;
                case 2:
                    // 左側
                    // active_mode=4でラインを跨いで右側にいるので、左ターン
                    mBalancingWalker->blindWalk(-6, 6);
                    break;
                default:
                    break;
            }
        } else {
            active_mode = 6;
            cyc_cnt = 0;
            Distance_init();
        }
    }

    if (active_mode == 6) {
        // 最後のライントレース
        // グレー検出ができないので距離は決め打ち
        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        int grayScaleValue = mNavigator->getGrayScale();
        int threshold = (onGroundMax.value + onGroundMin.value) / 2;

        int thresholdOffset = 20;
        threshold += thresholdOffset;
        
        int diff = grayScaleValue - threshold;

        if (diff > 80) {
            diff = 80;
        } else if (diff < -80) {
            diff = -80;
        }

        // 2017.9.27のレプリカコース試走では5cm程度オーバーラン気味
        // ライン大幅ズレの場合はショートラン気味もう2cmあるとよい
        // if (distance < 600) {
        if (distance < distanceToGarage) {

            diff /= 2;

            int forwardOffsetL = 0;
            int forwardOffsetR = 0;

            int inverted = 1;

            if (groundEnvironment == 2 || groundEnvironment == 0) {
                inverted = -1;
            }

            if (diff > 0) {
                forwardOffsetL = diff * inverted;
                forwardOffsetR = 0;
            } else {
                forwardOffsetL = 0;
                forwardOffsetR = -diff * inverted;
            }
            if (((cyc_cnt - 1) % 10) == 0) {
                // 10回に1回だけパラメータ更新
                mBalancingWalker->blindWalk(10 + forwardOffsetL, 10 + forwardOffsetR);
            }
        } else {
            mBalancingWalker->blindWalk(0, 0);
        }
    }

    if (active_mode == 7) {
        // 右に大幅ズレで左90度ターン＆前進後、ラインを発見
        // 右90度スピンして、active_mode=2から
        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        if (distanceL < 127) {
            mBalancingWalker->blindWalk(6, -6);
        } else {
            // 最大最小もリセット
            onGroundMax.time = 0;
            onGroundMax.value = 0;
            onGroundMax.distance = 0;
            onGroundMin.time = 0;
            onGroundMin.value = 1000;
            onGroundMin.distance = 0;
                    
            active_mode = 2;
            cyc_cnt = 0;
            Distance_init();
        }
    
    }

    if (active_mode == 8) {
        // 左に大幅ズレで左90度ターン＆前進後、緑地を発見し、
        // 緑なので、180°右ターンして
        // まっすぐ進んで、黒っぽいのが見つかったらactive_mode=9

        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        if (distanceL < 254) {
            mBalancingWalker->blindWalk(6, -6);
        } else {
            mBalancingWalker->blindWalk(8, 8);
            int grayScaleValue = mNavigator->getGrayScale();
            
            // if ((onGroundMax.value - grayScaleValue) > 50) {
            if (grayScaleValue < 15) {
                active_mode = 9;
                cyc_cnt = 0;
                Distance_init();
            }
        }
    }

    if (active_mode == 9) {
        // 左に大幅ズレで左90度ターン＆前進後、緑地を発見し、
        // 180度ターン後、黒ラインまで前進
        // 左90度スピンして、active_mode=2から
        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        if (distanceR < 127) {
            mBalancingWalker->blindWalk(-6, 6);
        } else {
            // 最大最小もリセット
            onGroundMax.time = 0;
            onGroundMax.value = 0;
            onGroundMax.distance = 0;
            onGroundMin.time = 0;
            onGroundMin.value = 1000;
            onGroundMin.distance = 0;
        
            active_mode = 2;
            cyc_cnt = 0;
            Distance_init();
        }
    
    }

    if (active_mode == 10) {
        // LコースGarageInからの移行

        ++cyc_cnt;
        Distance_update();
        tail_control(TAIL_ANGLE_DETECT_LINE);

        if (cyc_cnt > 1000) {
            // 連続動作だと転倒のおそれがあるので、時間を空ける
            active_mode = 2;
            Distance_init();
            cyc_cnt = 0;
        }
    }

    mNavigator->setActiveMode(active_mode);

    return 0;
}

/* 初期化関数 */
void GarageInR::Distance_init() {
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
void GarageInR::Distance_update(){
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
float GarageInR::Distance_getDistance(){
    return distance;
}

/* 右タイヤの4ms間の距離を取得 */
float GarageInR::Distance_getDistance4msRight(){
    return distance4msR;
}

/* 左タイヤの4ms間の距離を取得 */
float GarageInR::Distance_getDistance4msLeft(){
    return distance4msL;
}

int GarageInR::judgeEnvironment(GrayScaleOnGround max, GrayScaleOnGround min) {

    int result = 0;

    // if ((max.value - min.value) > 50) {
    if (min.value < 15) {
            // ラインらしきものを見つけている
        if (min.distance < 50) {
        // if (min.time < 500) {
                // かなり近い
            result = 0;
        } else if (min.distance < 200) {
            // ほんの少し左ズレの場合、23cmくらいでピークが来るので
        // } else if (min.distance < 320) {
        // } else if (min.time < 3000) {
            // 右側にズレているので、比較的早く見つかる
            result = 1;
        } else {
            // 左側にズレているので、見つかるまで結構時間かかる
            result = 2;
        }
    } else {
        // ラインらしきものがない
        result = 3;
    }
    return result;
}


void GarageInR::setDistanceToGarage(int distanceToGarage) {
    GarageInR::distanceToGarage = distanceToGarage;
}

int GarageInR::getDistanceToGarage() {
    return distanceToGarage;
}

void GarageInR::setActiveMode(int activeMode) {
    active_mode = activeMode;
}

int GarageInR::getActiveMode() {
    return active_mode;
}


