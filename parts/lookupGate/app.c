/**
 ******************************************************************************
 ** ファイル名 : app.c
 **
 ** 概要 : 2輪倒立振子ライントレースロボットのTOPPERS/HRP2用Cサンプルプログラム
 **
 ** 注記 : sample_c4 (sample_c3にBluetooth通信リモートスタート機能を追加)
 ******************************************************************************
 **/

#include "ev3api.h"
#include "app.h"
#include "balancer.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/**
 * センサー、モーターの接続を定義します
 */
static const sensor_port_t
    touch_sensor    = EV3_PORT_1,
    sonar_sensor    = EV3_PORT_2,
    color_sensor    = EV3_PORT_3,
    gyro_sensor     = EV3_PORT_4;

static const motor_port_t
    left_motor      = EV3_PORT_C,
    right_motor     = EV3_PORT_B,
    tail_motor      = EV3_PORT_A;

static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */

/* 下記のマクロは個体/環境に合わせて変更する必要があります */
/* sample_c1マクロ */
#define GYRO_OFFSET  0          /* ジャイロセンサオフセット値(角速度0[deg/sec]時) */
#define LIGHT_WHITE  62         /* 白色の光センサ値 */
#define LIGHT_BLACK  2          /* 黒色の光センサ値 */
/* sample_c2マクロ */
#define SONAR_ALERT_DISTANCE 30 /* 超音波センサによる障害物検知距離[cm] */
/* sample_c3マクロ */
#define TAIL_ANGLE_STAND_UP  91 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */
#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */
/* sample_c4マクロ */
//#define DEVICE_NAME     "ET0"  /* Bluetooth名 hrp2/target/ev3.h BLUETOOTH_LOCAL_NAMEで設定 */
//#define PASS_KEY        "1234" /* パスキー    hrp2/target/ev3.h BLUETOOTH_PIN_CODEで設定 */
#define CMD_START         '1'    /* リモートスタートコマンド */

/* ----------------- 東北地区実行委員会　追記箇所　ここから ------------- */

/* PID 制御用　マクロ定義 */
#define INTERVAL 0.004 /* 制御間隔 4 [ms] */
#define TARGET_VAL ((LIGHT_BLACK + LIGHT_WHITE) / 2) /* 目標値 */
#define TURN_MAX 100 /* 操作量の最大値 */
#define TURN_MIN (-100) /* 操作量の最小値 */

// #define KP 2.00 /* 比例ゲイン : 右エッジの場合は負の値 */
// #define TI 10000000.0 /* 積分時間 */
// #define TD 0.0 /* 微分時間 */

#define KC 3.00 /* 限界感度法による持続振動時の比例ゲイン */
#define TC 0.484 /* 限界感度法による持続振動の周期 */
#define KP (0.6 * KC) /* 比例動作の比例係数 */
#define TI (0.5 * TC) /* 積分動作の比例係数 */
#define TD (0.125 * TC) /* 微分動作の比例係数 */

/* ----------------- 東北地区実行委員会　追記箇所　ここまで ------------- */
/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

/* 関数プロトタイプ宣言 */
static int sonar_alert(void);
static void tail_control(signed int angle);

/* メインタスク */
void main_task(intptr_t unused)
{
    signed char forward;      /* 前後進命令 */
    signed char turn;         /* 旋回命令 */
    signed char pwm_L, pwm_R; /* 左右モータPWM出力 */
    char turnval[3];
    int ref = 0;

    /* ----------------- 東北地区実行委員会　追記箇所　ここから ------------- */

    /* PID 制御用　変数宣言 */
    float e_t; /* 偏差 */
    float der_e_t; /* 偏差の微分項 */
    float int_e_t; /* 偏差の積分項 */
    float prev_e_t; /* 1 ステップ前の偏差 */
    float u_t; /* 操作量計算領域（実数） */
    int temp_turn; /* 操作量計算領域（整数） */
    unsigned short int m_val; /* 観測値 */


    /* ----------------- 東北地区実行委員会　追記箇所　ここまで ------------- */

    /* LCD画面表示 */
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    ev3_lcd_draw_string("EV3way-ET sample_c4", 0, CALIB_FONT_HEIGHT*1);

    /* センサー入力ポートの設定 */
    ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);
    ev3_color_sensor_get_reflect(color_sensor); /* 反射率モード */
    ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
    ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
    /* モーター出力ポートの設定 */
    ev3_motor_config(left_motor, LARGE_MOTOR);
    ev3_motor_config(right_motor, LARGE_MOTOR);
    ev3_motor_config(tail_motor, LARGE_MOTOR);
    ev3_motor_reset_counts(tail_motor);

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);

    ev3_led_set_color(LED_ORANGE); /* 初期化完了通知 */

    /* スタート待機 */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */

        if (bt_cmd == 1)
        {
            break; /* リモートスタート */
        }

        if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
        {
            break; /* タッチセンサが押された */
        }

        tslp_tsk(10); /* 10msecウェイト */
    }

    /* ----------------- 東北地区実行委員会　追記箇所　ここから ------------- */

    /* PID 制御用　初期設定 */
    /* 1 ステップ前の偏差の初期値を設定 */
    prev_e_t = (float)(TARGET_VAL - ev3_color_sensor_get_reflect(color_sensor));
    int_e_t = 0.0; /* 積分項の累積値をゼロクリア */

    /* 走行モーターエンコーダーリセット */
    ev3_motor_reset_counts(left_motor);
    ev3_motor_reset_counts(right_motor);

    /* ジャイロセンサーリセット */
    ev3_gyro_sensor_reset(gyro_sensor);
    balance_init(); /* 倒立振子API初期化 */

    ev3_led_set_color(LED_GREEN); /* スタート通知 */

    /**
    * Main loop for the self-balance control algorithm
    */
    while(1)
    {
        int32_t motor_ang_l, motor_ang_r;
        int gyro, volt;
	    // asumis
        static int32_t cnt = 0;
	    char msg[100];
        // asumie
        if (ev3_button_is_pressed(BACK_BUTTON)) break;
        if(bt_cmd == 0){
            //asumis
            sprintf(msg, "%d", (int)cnt);
            ev3_lcd_draw_string(msg,75,62);
            //asumie
            break;
        }

        tail_control(TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */

        if (sonar_alert() == 1) /* 障害物検知 */
        {
            forward = turn = 0; /* 障害物を検知したら停止 */
        }
        else
        {
            forward = 80; /* 前進命令 */
            
        	cnt = ev3_motor_get_counts(left_motor) * -1;   //asumi
        }
        /* ----------------- 東北地区実行委員会　修正箇所　ここから ------------- */

        /* PID 制御用　操作量の算出 */
        /* 観測値を取得 */
        m_val = ev3_color_sensor_get_reflect(color_sensor);
        ref = m_val;
        e_t = (float)(TARGET_VAL - m_val); /* 偏差を算出 */
        int_e_t = int_e_t + e_t * INTERVAL; /* 積分項を算出 */
        der_e_t = (e_t - prev_e_t) / INTERVAL; /* 微分項を算出 */
        u_t = KP * (e_t + int_e_t / TI + TD * der_e_t); /* 操作量を計算 */
        temp_turn = (int)u_t; /* 操作量を整数化 */
        if (temp_turn > TURN_MAX) /* 操作量の上限設定 */
        temp_turn = TURN_MAX;
        if (temp_turn < TURN_MIN) /* 操作量の加減設定 */
        temp_turn = TURN_MIN;
        turn = (signed char)temp_turn; /* 操作量を確定 */

        /* 現在の偏差を次回の 1 ステップ前の偏差とする */
        prev_e_t = e_t;

        /* 倒立振子制御API に渡すパラメータを取得する */
        motor_ang_l = ev3_motor_get_counts(left_motor);
        motor_ang_r = ev3_motor_get_counts(right_motor);
        gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
        volt = ev3_battery_voltage_mV();

        /* 倒立振子制御APIを呼び出し、倒立走行するための */
        /* 左右モータ出力値を得る */
        balance_control(
            (float)forward,
            (float)turn,
            (float)gyro,
            (float)GYRO_OFFSET,
            (float)motor_ang_l,
            (float)motor_ang_r,
            (float)volt,
            (signed char*)&pwm_L,
            (signed char*)&pwm_R);

        /* EV3ではモーター停止時のブレーキ設定が事前にできないため */
        /* 出力0時に、その都度設定する */
        if (pwm_L == 0)
        {
             ev3_motor_stop(left_motor, true);
        }
        else
        {
            ev3_motor_set_power(left_motor, (int)pwm_L);
        }
        
        if (pwm_R == 0)
        {
             ev3_motor_stop(right_motor, true);
        }
        else
        {
            ev3_motor_set_power(right_motor, (int)pwm_R);
        }

        sprintf(turnval,"%d,",turn);
        int i = 0;
        while(turnval[i])
            fputc(turnval[i++], bt);

        tslp_tsk(4); /* 4msec周期起動 */
    }
    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// 関数名 : sonar_alert
// 引数 : 無し
// 返り値 : 1(障害物あり)/0(障害物無し)
// 概要 : 超音波センサによる障害物検知
//*****************************************************************************
static int sonar_alert(void)
{
    static unsigned int counter = 0;
    static int alert = 0;

    signed int distance;

    if (++counter == 40/4) /* 約40msec周期毎に障害物検知  */
    {
        /*
         * 超音波センサによる距離測定周期は、超音波の減衰特性に依存します。
         * NXTの場合は、40msec周期程度が経験上の最短測定周期です。
         * EV3の場合は、要確認
         */
        distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
        if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
        {
            alert = 1; /* 障害物を検知 */
        }
        else
        {
            alert = 0; /* 障害物無し */
        }
        counter = 0;
    }

    return alert;
}

//*****************************************************************************
// 関数名 : tail_control
// 引数 : angle (モータ目標角度[度])
// 返り値 : 無し
// 概要 : 走行体完全停止用モータの角度制御
//*****************************************************************************
static void tail_control(signed int angle)
{
    float pwm = (float)(angle - ev3_motor_get_counts(tail_motor))*P_GAIN; /* 比例制御 */
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
        ev3_motor_stop(tail_motor, true);
    }
    else
    {
        ev3_motor_set_power(tail_motor, (signed char)pwm);
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
        case '1':
            bt_cmd = 1;
            break;
        case '0':
            bt_cmd = 0;
            break;
        default:
            break;
        }
        // fputc(c, bt); /* エコーバック */
    }
}
