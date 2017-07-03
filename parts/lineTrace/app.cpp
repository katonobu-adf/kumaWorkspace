/******************************************************************************
 *  app.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "LineTracer.h"

// ‰œŽR’Ç‰Á <begin>
#include "TouchSensor.h"
// ‰œŽR’Ç‰Á <end>

// ƒfƒXƒgƒ‰ƒNƒ^–â‘è‚Ì‰ñ”ð
// https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping
void *__dso_handle=0;

// usingéŒ¾
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;

// ‰œŽR’Ç‰Á <begin>
using ev3api::TouchSensor;

#define TAIL_ANGLE_STAND_UP  92 /* Š®‘S’âŽ~Žž‚ÌŠp“x[“x] */
#define TAIL_ANGLE_DRIVE      3 /* ƒoƒ‰ƒ“ƒX‘–sŽž‚ÌŠp“x[“x] */
#define P_GAIN             2.5F /* Š®‘S’âŽ~—pƒ‚[ƒ^§Œä”ä—áŒW” */
#define PWM_ABS_MAX          60 /* Š®‘S’âŽ~—pƒ‚[ƒ^§ŒäPWMâ‘ÎÅ‘å’l */

static int      bt_cmd = 0;     /* BluetoothƒRƒ}ƒ“ƒh 1:ƒŠƒ‚[ƒgƒXƒ^[ƒg */
static FILE     *bt = NULL;     /* Bluetoothƒtƒ@ƒCƒ‹ƒnƒ“ƒhƒ‹ */

// ‰œŽR’Ç‰Á <end>

// ²X–Ø’Ç‰Á <begin>
#define RUN_PER_SEC 250
#define RUN_TIME    RUN_PER_SEC * 4
int run_various = 0;
int run_cnt = 0;
int bright_val = 0;
int ambient_val = 0;
char light_msg[32];

// ²X–Ø’Ç‰Á <end>



// Device objects
// ƒIƒuƒWƒFƒNƒg‚ðÃ“I‚ÉŠm•Û‚·‚é
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);

// ‰œŽR’Ç‰Á <begin>
TouchSensor gTouchSensor(PORT_1);
Motor       gTail(PORT_A);

static void readyToStart(void);
static void tail_control(signed int angle);
// ‰œŽR’Ç‰Á <end>

// ƒIƒuƒWƒFƒNƒg‚Ì’è‹`
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;

/**
 * EV3ƒVƒXƒeƒ€¶¬
 */
static void user_system_create() {
    // ƒIƒuƒWƒFƒNƒg‚Ìì¬
    gBalancer        = new Balancer();
    gBalancingWalker = new BalancingWalker(gGyroSensor,
                                           gLeftWheel,
                                           gRightWheel,
                                           gBalancer);
    gLineMonitor     = new LineMonitor(gColorSensor);
    gLineTracer      = new LineTracer(gLineMonitor, gBalancingWalker);
// ‰œŽR’Ç‰Á <begin>
    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth’ÊMƒ^ƒXƒN‚Ì‹N“® */
    act_tsk(BT_TASK);
// ‰œŽR’Ç‰Á <end>

// ²X–Ø’Ç‰Á <begin>
    gTail.setBrake(true); // ƒuƒŒ[ƒLƒ‚[ƒhÝ’è
    gTail.reset();
// ²X–Ø’Ç‰Á <end>

}

/**
 * EV3ƒVƒXƒeƒ€”jŠü
 */
static void user_system_destroy() {
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracer;
    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;

// ‰œŽR’Ç‰Á <begin>
    ter_tsk(BT_TASK);
    fclose(bt);
// ‰œŽR’Ç‰Á <end>
}

/**
 * ƒƒCƒ“ƒ^ƒXƒN
 */
void main_task(intptr_t unused) {
    user_system_create();  // ƒZƒ“ƒT‚âƒ‚[ƒ^‚Ì‰Šú‰»ˆ—

    // ²X–Ø’Ç‰Á <begin>
    while(true){
        readyToStart();
        if(ev3_button_is_pressed(BACK_BUTTON)){
            break; // ŒJ‚è•Ô‚µI—¹
        }
        
        ev3_led_set_color(LED_GREEN);
        // ŽüŠúƒnƒ“ƒhƒ‰ŠJŽn
        switch(run_various){
            case 1:
                ev3_sta_cyc(EV3_CYC_AMBIENT);
                slp_tsk();  // u‚Ov‚ª“ü—Í‚³‚ê‚é‚©Aˆê’èŽžŠÔŒo‰ß‚µ‚½‚ç‰ðœ
                ev3_stp_cyc(EV3_CYC_AMBIENT); // ŽüŠúƒnƒ“ƒhƒ‰’âŽ~
                break;
            case 2:
                ev3_sta_cyc(EV3_CYC_BRIGHT);
                slp_tsk();  // u‚Ov‚ª“ü—Í‚³‚ê‚é‚©Aˆê’èŽžŠÔŒo‰ß‚µ‚½‚ç‰ðœ
                ev3_stp_cyc(EV3_CYC_BRIGHT); // ŽüŠúƒnƒ“ƒhƒ‰’âŽ~
                break;
            default:
                break;
        }
    }
    user_system_destroy();  // I—¹ˆ—
    ext_tsk();
}

/**
 * Œv‘ªŽÀsƒ^ƒCƒ~ƒ“ƒO
 */
void ev3_cyc_ambient(intptr_t exinf) {
    act_tsk(MEASURE_AMBIENT_TASK);
}

/**
 * ŠÂ‹«ŒõŒv‘ªƒ^ƒXƒN
 */
void measure_ambient_task(intptr_t exinf) {
    tail_control(TAIL_ANGLE_STAND_UP); // ÃŽ~ó‘Ô‚Ì‚Ü‚ÜŒv‘ª

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // ƒƒCƒ“ƒ^ƒXƒNÄŠJiŒv‘ªƒ^ƒXƒN‚ÌI—¹j

    } else {
        ambient_val = gLineMonitor->getAmbient(); // ŠÂ‹«Œõ‚ðŒv‘ª
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
    tail_control(TAIL_ANGLE_STAND_UP); // ÃŽ~ó‘Ô‚Ì‚Ü‚ÜŒv‘ª

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // ƒƒCƒ“ƒ^ƒXƒNÄŠJiŒv‘ªƒ^ƒXƒN‚ÌI—¹j

    } else {
        bright_val = gLineMonitor->getBrightness(); // ”½ŽËŒõ‚ðŒv‘ª
        sprintf(light_msg, "%d, ", bright_val);
        fputs(light_msg, bt);
        ++run_cnt;
    }
    ext_tsk();
}

// ‰œŽR’Ç‰Á <begin>
static void  readyToStart(void){
    ev3_led_set_color(LED_ORANGE);
    bt_cmd = 0;
    
    /* ƒXƒ^[ƒg‘Ò‹@ */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP); /* Š®‘S’âŽ~—pŠp“x‚É§Œä */
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
            break; /* ƒŠƒ‚[ƒgƒXƒ^[ƒg */
        }

        tslp_tsk(10);         /* 10msecƒEƒFƒCƒg */
    }
}

//*****************************************************************************
// ŠÖ”–¼ : tail_control
// ˆø” : angle (ƒ‚[ƒ^–Ú•WŠp“x[“x])
// •Ô‚è’l : –³‚µ
// ŠT—v : ‘–s‘ÌŠ®‘S’âŽ~—pƒ‚[ƒ^‚ÌŠp“x§Œä
//*****************************************************************************
static void tail_control(signed int angle)
{
    float pwm = (float)(angle - gTail.getCount() )*P_GAIN; /* ”ä—á§Œä */
    /* PWMo—Í–O˜aˆ— */
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
// ŠÖ”–¼ : bt_task
// ˆø” : unused
// •Ô‚è’l : ‚È‚µ
// ŠT—v : Bluetooth’ÊM‚É‚æ‚éƒŠƒ‚[ƒgƒXƒ^[ƒgB Tera Term‚È‚Ç‚Ìƒ^[ƒ~ƒiƒ‹ƒ\ƒtƒg‚©‚çA
//       ASCIIƒR[ƒh‚Å1‚ð‘—M‚·‚é‚ÆAƒŠƒ‚[ƒgƒXƒ^[ƒg‚·‚éB
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
        uint8_t c = fgetc(bt); /* ŽóM */
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
        fputc(c, bt); /* ƒGƒR[ƒoƒbƒN */
    }
}
// ‰œŽR’Ç‰Á <end>