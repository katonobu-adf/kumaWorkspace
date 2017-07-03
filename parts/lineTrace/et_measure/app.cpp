/******************************************************************************
 *  app.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "LineTracer.h"

// ���R�ǉ� <begin>
#include "TouchSensor.h"
// ���R�ǉ� <end>

// �f�X�g���N�^���̉��
// https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping
void *__dso_handle=0;

// using�錾
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;

// ���R�ǉ� <begin>
using ev3api::TouchSensor;

#define TAIL_ANGLE_STAND_UP  92 /* ���S��~���̊p�x[�x] */
#define TAIL_ANGLE_DRIVE      3 /* �o�����X���s���̊p�x[�x] */
#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          60 /* ���S��~�p���[�^����PWM��΍ő�l */

static int      bt_cmd = 0;     /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;     /* Bluetooth�t�@�C���n���h�� */

// ���R�ǉ� <end>

// ���X�ؒǉ� <begin>
#define RUN_PER_SEC 250
#define RUN_TIME    RUN_PER_SEC * 4
int run_various = 0;
int run_cnt = 0;
int bright_val = 0;
int ambient_val = 0;
char light_msg[32];

// ���X�ؒǉ� <end>



// Device objects
// �I�u�W�F�N�g��ÓI�Ɋm�ۂ���
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);

// ���R�ǉ� <begin>
TouchSensor gTouchSensor(PORT_1);
Motor       gTail(PORT_A);

static void readyToStart(void);
static void tail_control(signed int angle);
// ���R�ǉ� <end>

// �I�u�W�F�N�g�̒�`
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;

/**
 * EV3�V�X�e������
 */
static void user_system_create() {
    // �I�u�W�F�N�g�̍쐬
    gBalancer        = new Balancer();
    gBalancingWalker = new BalancingWalker(gGyroSensor,
                                           gLeftWheel,
                                           gRightWheel,
                                           gBalancer);
    gLineMonitor     = new LineMonitor(gColorSensor);
    gLineTracer      = new LineTracer(gLineMonitor, gBalancingWalker);
// ���R�ǉ� <begin>
    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth�ʐM�^�X�N�̋N�� */
    act_tsk(BT_TASK);
// ���R�ǉ� <end>

// ���X�ؒǉ� <begin>
    gTail.setBrake(true); // �u���[�L���[�h�ݒ�
    gTail.reset();
// ���X�ؒǉ� <end>

}

/**
 * EV3�V�X�e���j��
 */
static void user_system_destroy() {
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracer;
    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;

// ���R�ǉ� <begin>
    ter_tsk(BT_TASK);
    fclose(bt);
// ���R�ǉ� <end>
}

/**
 * ���C���^�X�N
 */
void main_task(intptr_t unused) {
    user_system_create();  // �Z���T�⃂�[�^�̏���������

    // ���X�ؒǉ� <begin>
    while(true){
        readyToStart();
        if(ev3_button_is_pressed(BACK_BUTTON)){
            break; // �J��Ԃ��I��
        }
        
        ev3_led_set_color(LED_GREEN);
        // �����n���h���J�n
        switch(run_various){
            case 1:
                ev3_sta_cyc(EV3_CYC_AMBIENT);
                slp_tsk();  // �u�O�v�����͂���邩�A��莞�Ԍo�߂��������
                ev3_stp_cyc(EV3_CYC_AMBIENT); // �����n���h����~
                break;
            case 2:
                ev3_sta_cyc(EV3_CYC_BRIGHT);
                slp_tsk();  // �u�O�v�����͂���邩�A��莞�Ԍo�߂��������
                ev3_stp_cyc(EV3_CYC_BRIGHT); // �����n���h����~
                break;
            default:
                break;
        }
    }
    user_system_destroy();  // �I������
    ext_tsk();
}

/**
 * �v�����s�^�C�~���O
 */
void ev3_cyc_ambient(intptr_t exinf) {
    act_tsk(MEASURE_AMBIENT_TASK);
}

/**
 * �����v���^�X�N
 */
void measure_ambient_task(intptr_t exinf) {
    tail_control(TAIL_ANGLE_STAND_UP); // �Î~��Ԃ̂܂܌v��

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // ���C���^�X�N�ĊJ�i�v���^�X�N�̏I���j

    } else {
        ambient_val = gLineMonitor->getAmbient(); // �������v��
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
    tail_control(TAIL_ANGLE_STAND_UP); // �Î~��Ԃ̂܂܌v��

    if (bt_cmd == 0 || run_cnt >= RUN_TIME) {
        run_cnt = 0;
        wup_tsk(MAIN_TASK);  // ���C���^�X�N�ĊJ�i�v���^�X�N�̏I���j

    } else {
        bright_val = gLineMonitor->getBrightness(); // ���ˌ����v��
        sprintf(light_msg, "%d, ", bright_val);
        fputs(light_msg, bt);
        ++run_cnt;
    }
    ext_tsk();
}

// ���R�ǉ� <begin>
static void  readyToStart(void){
    ev3_led_set_color(LED_ORANGE);
    bt_cmd = 0;
    
    /* �X�^�[�g�ҋ@ */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */
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
            break; /* �����[�g�X�^�[�g */
        }

        tslp_tsk(10);         /* 10msec�E�F�C�g */
    }
}

//*****************************************************************************
// �֐��� : tail_control
// ���� : angle (���[�^�ڕW�p�x[�x])
// �Ԃ�l : ����
// �T�v : ���s�̊��S��~�p���[�^�̊p�x����
//*****************************************************************************
static void tail_control(signed int angle)
{
    float pwm = (float)(angle - gTail.getCount() )*P_GAIN; /* ��ᐧ�� */
    /* PWM�o�͖O�a���� */
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
// �֐��� : bt_task
// ���� : unused
// �Ԃ�l : �Ȃ�
// �T�v : Bluetooth�ʐM�ɂ�郊���[�g�X�^�[�g�B Tera Term�Ȃǂ̃^�[�~�i���\�t�g����A
//       ASCII�R�[�h��1�𑗐M����ƁA�����[�g�X�^�[�g����B
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
        uint8_t c = fgetc(bt); /* ��M */
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
        fputc(c, bt); /* �G�R�[�o�b�N */
    }
}
// ���R�ǉ� <end>