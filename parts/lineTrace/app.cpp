/******************************************************************************
 *  app.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "LineTracer.h"

// 奥山追加 <begin>
#include "TouchSensor.h"
// 奥山追加 <end>

// デストラクタ問題の回避
// https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping
void *__dso_handle=0;

// using宣言
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;

// 奥山追加 <begin>
using ev3api::TouchSensor;

#define TAIL_ANGLE_STAND_UP  92 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */
#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */

static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

// 奥山追加 <end>

// 佐々木追加 <begin>
#define RUN_PER_SEC 250
#define RUN_TIME    RUN_PER_SEC * 4
int run_various = 0;
int run_cnt = 0;
int bright_val = 0;
int ambient_val = 0;
char light_msg[32];

// 佐々木追加 <end>



// Device objects
// オブジェクトを静的に確保する
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);

// 奥山追加 <begin>
TouchSensor gTouchSensor(PORT_1);
Motor       gTail(PORT_A);

static void readyToStart(void);
static void tail_control(signed int angle);
// 奥山追加 <end>

// オブジェクトの定義
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;

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
// 奥山追加 <begin>
    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);
// 奥山追加 <end>

// 佐々木追加 <begin>
    gTail.setBrake(true); // ブレーキモード設定
    gTail.reset();
// 佐々木追加 <end>

}

/**
 * EV3システム破棄
 */
static void user_system_destroy() {
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracer;
    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;

// 奥山追加 <begin>
    ter_tsk(BT_TASK);
    fclose(bt);
// 奥山追加 <end>
}

/**
 * メインタスク
 */
void main_task(intptr_t unused) {
    user_system_create();  // センサやモータの初期化処理

    // 佐々木追加 <begin>
    while(true){
        readyToStart();
        if(ev3_button_is_pressed(BACK_BUTTON)){
            break; // 繰り返し終了
        }
        
        ev3_led_set_color(LED_GREEN);
        // 周期ハンドラ開始
        switch(run_various){
            case 1:
                ev3_sta_cyc(EV3_CYC_AMBIENT);
                slp_tsk();  // 「０」が入力されるか、一定時間経過したら解除
                ev3_stp_cyc(EV3_CYC_AMBIENT); // 周期ハンドラ停止
                break;
            case 2:
                ev3_sta_cyc(EV3_CYC_BRIGHT);
                slp_tsk();  // 「０」が入力されるか、一定時間経過したら解除
                ev3_stp_cyc(EV3_CYC_BRIGHT); // 周期ハンドラ停止
                break;
            default:
                break;
        }
    }
    user_system_destroy();  // 終了処理
    ext_tsk();
}

/**
 * 計測実行タイミング
 */
void ev3_cyc_ambient(intptr_t exinf) {
    act_tsk(MEASURE_AMBIENT_TASK);
}

/**
 * 環境光計測タスク
 */
void measure_ambient_task(intptr_t exinf) {
    tail_control(TAIL_ANGLE_STAND_UP); // 静止状態のまま計測

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // メインタスク再開（計測タスクの終了）

    } else {
        ambient_val = gLineMonitor->getAmbient(); // 環境光を計測
        sprintf(light_msg, "%d, ", ambient_val);
        fputs(light_msg, bt);
        ++run_cnt;
    }
    ext_tsk();
}

void ev3_cyc_bright(intptr_t exinf) {
    act_tsk(MEASURE_BRIGHT_TASK);
}

void measure_bright_task(intptr_t exinf) {
    tail_control(TAIL_ANGLE_STAND_UP); // 静止状態のまま計測

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // メインタスク再開（計測タスクの終了）

    } else {
        bright_val = gLineMonitor->getBrightness(); // 反射光を計測
        sprintf(light_msg, "%d, ", bright_val);
        fputs(light_msg, bt);
        ++run_cnt;
    }
    ext_tsk();
}

// 奥山追加 <begin>
static void  readyToStart(void){
    ev3_led_set_color(LED_ORANGE);
    bt_cmd = 0;
    
    /* スタート待機 */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */
        switch(bt_cmd){
            case 1:
                run_various = 1;
                break;
            case 2:
                run_various = 2;
                break;
            default:
                break; 
        }
        if (bt_cmd != 0 || ev3_button_is_pressed(BACK_BUTTON)){
            break; /* リモートスタート */
        }

        tslp_tsk(10);         /* 10msecウェイト */
    }
}

//*****************************************************************************
// 関数名 : tail_control
// 引数 : angle (モータ目標角度[度])
// 返り値 : 無し
// 概要 : 走行体完全停止用モータの角度制御
//*****************************************************************************
static void tail_control(signed int angle)
{
    float pwm = (float)(angle - gTail.getCount() )*P_GAIN; /* 比例制御 */
    /* PWM出力飽和処理 */
    if (pwm > PWM_ABS_MAX)
    {
        pwm = PWM_ABS_MAX;
    }
    else if (pwm < -PWM_ABS_MAX)
    {
        pwm = -PWM_ABS_MAX;
    }

    if (pwm == 0)
    {
        gTail.stop();
    }
    else
    {
        gTail.setPWM((int)pwm);
    }
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
        uint8_t c = fgetc(bt); /* 受信 */
        switch(c)
        {
        case '0':
            bt_cmd = 0;
            break;
        case '1':
            bt_cmd = 1;
            break;
        case '2':
            bt_cmd = 2;
            break;
        default:
            break;
        }
        fputc(c, bt); /* エコーバック */
    }
}
// 奥山追加 <end>