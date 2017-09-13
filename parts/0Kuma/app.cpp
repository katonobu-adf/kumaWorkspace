
/******************************************************************************
 *  app.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "SonarSensor.h"
#include "TouchSensor.h"

#include "app.h"
#include "Driver.h"
#include "Navigator.h"
#include "Ev3Logger.h"

// デストラクタ問題の回避
void *__dso_handle=0;   // https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping

// using宣言
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;

static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

// オブジェクトを静的に確保する
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);
TouchSensor gTouchSensor(PORT_1);
SonarSensor gSonarSensor(PORT_2);
Motor       gTail(PORT_A);

// オブジェクトの定義
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;

static Navigator       *gNavigator;
static Driver          *gDriver;

static Ev3Logger          *gEv3Logger;

static int prevState = 0;

/**
 * EV3システム生成
 */
static void user_system_create() {
    // オブジェクトの作成
    gBalancer        = new Balancer();
    gBalancingWalker = new BalancingWalker(gGyroSensor,
                                           gLeftWheel,
                                           gRightWheel,
                                           gBalancer);
    gLineMonitor     = new LineMonitor(gColorSensor, 
                                       Navigator::initialThreshold);

    gNavigator       = new Navigator(gLineMonitor, 
                                     gBalancingWalker,
                                     gTouchSensor,
                                     gSonarSensor);
    
    gDriver          = new Driver( gNavigator,
                                   gBalancingWalker,
                                   gTail);

	gEv3Logger = new Ev3Logger(gColorSensor, gGyroSensor, gTouchSensor, gSonarSensor, gTail, gLeftWheel, gRightWheel);

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);

    // 初期化完了通知
    ev3_led_set_color(LED_ORANGE);
    ev3_speaker_set_volume(1);
}

/**
 * EV3システム破棄
 */
static void user_system_destroy()
{
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;
    delete gNavigator;
    delete gDriver;
    delete gEv3Logger;

    ter_tsk(BT_TASK);
    fclose(bt);
}

/**
 * メインタスク
 */
void main_task(intptr_t unused)
{
    user_system_create();  // センサやモータの初期化処理

    ev3_sta_cyc(EV3_CYC_TASK_RUNER); // 周期ハンドラ開始
    slp_tsk(); // スリープ

    ev3_stp_cyc(EV3_CYC_TASK_RUNER); // 周期ハンドラ終了
    user_system_destroy();  // 終了処理

    ext_tsk();
}

/**
 * 各種タスクを実行する周期ハンドラ
 */
void ev3_cyc_task_runer(intptr_t)
{
    act_tsk(RACER_TASK);
}

/**
 * ライントレースタスク
 */
void racer_task(intptr_t exinf)
{
    int ret;
    int currState;

    // バックボタン押下？
    if ( ev3_button_is_pressed(BACK_BUTTON) ) {
        wup_tsk(MAIN_TASK);       // MainTask 起床 (このTaskはもう呼ばれない)
    } else {
        currState = gNavigator->getState();

    	if (currState >2) {
            if (prevState == 2) {
                gEv3Logger->resetTime();
            }

            int state = gDriver->getState();
            int forward = gBalancingWalker->getForward();
            int turn = gBalancingWalker->getTurn();
            gEv3Logger->logging(state, forward, turn);
    	} 

        // Driverオブジェクトへ走行するように伝える。
        ret = gDriver->run();

        // 終了のきっかけがあったか？
        if ( ret == 1 ){
            wup_tsk(MAIN_TASK);   // MainTask 起床 (このTaskはもう呼ばれない)
        }

        prevState = currState;
    }
    ext_tsk();
}

//*****************************************************************************
// 関数名 : bt_task
// 引数 : unused
// 返り値 : なし
// 概要 : Bluetooth通信によるリモートスタート。 Tera Termなどのターミナルソフトから、
//       ASCIIコードで1を送信すると、リモートスタートする。
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
        uint8_t c = fgetc(bt);  /* 受信 */
        switch(c){
            case '1' :
                gNavigator->setBtCommand(1);
                break;
            default:
                break;
        }
    }
}
