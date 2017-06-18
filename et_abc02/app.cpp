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

static const float  wheel_r = 8.3;
static const float  round_len = wheel_r * 3.1415;
static int    gLeftMeter;
static int    gRightMeter;

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

    // �����������ʒm
    ev3_led_set_color(LED_ORANGE);
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
 * �g���[�X���s�^�C�~���O
 */
void ev3_cyc_tracer(intptr_t exinf) {
    act_tsk(TRACER_TASK);
    act_tsk(MONITOR_TASK);
}

/**
 * ���C���^�X�N
 */
void main_task(intptr_t unused) {
    user_system_create();  // �Z���T�⃂�[�^�̏���������

// ���R�ǉ� <begin>
    readyToStart();
// ���R�ǉ� <end>

    // �����n���h���J�n
    ev3_sta_cyc(EV3_CYC_TRACER);

    slp_tsk();  // �o�b�N�{�^�����������܂ő҂�

    // �����n���h����~
    ev3_stp_cyc(EV3_CYC_TRACER);

    user_system_destroy();  // �I������

    ext_tsk();
}

/**
 * ���C���g���[�X�^�X�N
 */
void tracer_task(intptr_t exinf) {
    if (ev3_button_is_pressed(BACK_BUTTON)) {
        wup_tsk(MAIN_TASK);  // �o�b�N�{�^������
    } else {
        // ���s�����̌v��
        if ( ((gLeftMeter+gRightMeter)*round_len/360) / 2 < 100 ){
            tail_control(TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */
            gLineTracer->run();  // �|�����s
        } else {
            tail_control(TAIL_ANGLE_STAND_UP); /* ����������� */
            gLineTracer->stop(); // Stop
        }
    }

    ext_tsk();
}

// ���R�ǉ� <begin>
static void  readyToStart(void){

    gTail.setBrake(true); // �u���[�L���[�h�ݒ�
    gTail.reset();
    
    /* �X�^�[�g�ҋ@ */
    while(1)
    {
        tail_control(TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */

        if (bt_cmd == 1)
        {
            break; /* �����[�g�X�^�[�g */
        }

        if (gTouchSensor.isPressed() == 1)
        {
            break; /* �^�b�`�Z���T�������ꂽ */
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
        case '1':
            bt_cmd = 1;
            break;
        default:
            break;
        }
        fputc(c, bt); /* �G�R�[�o�b�N */
    }
}

void monitor_task(intptr_t unused)
{
    gLeftMeter  = gBalancingWalker->getLeftRound();
    gRightMeter = gBalancingWalker->getRightRound();
    ext_tsk();
}
// ���R�ǉ� <end>