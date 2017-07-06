/******************************************************************************
 *  app.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "LineTracer.h"
#include "TouchSensor.h"
#include "ReadyToStart.h"
#include "Driver.h"

// デストラクタ問題の回避
void *__dso_handle=0;   // https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping

// using宣言
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;
using ev3api::TouchSensor;

#define TAIL_ANGLE_STAND_UP  92 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */

#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */

static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

// 佐々木<begin>
int state = 0;            /* task_runer 内で実行するタスクを決める変数 */
// 佐々木<end>

// オブジェクトを静的に確保する
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);
TouchSensor gTouchSensor(PORT_1);
Motor       gTail(PORT_A);

// オブジェクトの定義
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;
       Driver          *gDriver;

// 佐々木<begin>
static ReadyToStart    *gReadyToStart;
// 佐々木<end>

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
    gLineMonitor     = new LineMonitor(gColorSensor);
    gLineTracer      = new LineTracer(gLineMonitor, gBalancingWalker);

    gDriver          = new Driver();

// 佐々木<begin>
    gReadyToStart    = new ReadyToStart(gTail);
// 佐々木<end>

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);

    // 初期化完了通知
    ev3_led_set_color(LED_ORANGE);
}

/**
 * EV3システム破棄
 */
static void user_system_destroy()
{
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracer;
    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;

    ter_tsk(BT_TASK);
    fclose(bt);
}

// 佐々木<begin>

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

    gDriver->run();

    // switch(state){
    //     case 0:
    //         act_tsk(READY_TASK);        
    //         break;
    //     case 1:
    //         act_tsk(TRACER_TASK);
    //         break;
    //     case 2:
    //         wup_tsk(MAIN_TASK);
    //         break;
    //     default:
    //         break;
    // }
}

/**
 * 走行体を静止させるタスク
 */
void ready_task(intptr_t exinf)
{
    gReadyToStart->run( TAIL_ANGLE_STAND_UP ); // 尻尾を降ろし、走行体を固定する
    if(bt_cmd == 1 || gTouchSensor.isPressed() == 1)
    {
        state = 1; // state を 0 から 1 へ移行
    }
}

// 佐々木<end>

/**
 * ライントレースタスク
 */
void tracer_task(intptr_t exinf)
{
    if ( ev3_button_is_pressed(BACK_BUTTON) ) {
        state = 2;  // バックボタン押下(state を 1 から 2 へ移行)
    } else {
        //tail_control(TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */
        gLineTracer->run();  // 倒立走行
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
            bt_cmd = 1;
            break;
        default:
            break;
        }
        fputc(c, bt); /* エコーバック */
    }
}
