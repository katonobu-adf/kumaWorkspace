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

// �f�X�g���N�^���̉��
void *__dso_handle=0;   // https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping

// using�錾
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;
using ev3api::TouchSensor;

#define TAIL_ANGLE_STAND_UP  92 /* ���S��~���̊p�x[�x] */
#define TAIL_ANGLE_DRIVE      3 /* �o�����X���s���̊p�x[�x] */

#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          60 /* ���S��~�p���[�^����PWM��΍ő�l */

static int      bt_cmd = 0;     /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;     /* Bluetooth�t�@�C���n���h�� */

// ���X��<begin>
int state = 0;            /* task_runer ���Ŏ��s����^�X�N�����߂�ϐ� */
// ���X��<end>

// �I�u�W�F�N�g��ÓI�Ɋm�ۂ���
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);
TouchSensor gTouchSensor(PORT_1);
Motor       gTail(PORT_A);

// �I�u�W�F�N�g�̒�`
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;
       Driver          *gDriver;

// ���X��<begin>
static ReadyToStart    *gReadyToStart;
// ���X��<end>

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

    gDriver          = new Driver();

// ���X��<begin>
    gReadyToStart    = new ReadyToStart(gTail);
// ���X��<end>

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth�ʐM�^�X�N�̋N�� */
    act_tsk(BT_TASK);

    // �����������ʒm
    ev3_led_set_color(LED_ORANGE);
}

/**
 * EV3�V�X�e���j��
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

// ���X��<begin>

/**
 * ���C���^�X�N
 */
void main_task(intptr_t unused)
{
    user_system_create();  // �Z���T�⃂�[�^�̏���������
    
    ev3_sta_cyc(EV3_CYC_TASK_RUNER); // �����n���h���J�n
    slp_tsk(); // �X���[�v

    ev3_stp_cyc(EV3_CYC_TASK_RUNER); // �����n���h���I��
    user_system_destroy();  // �I������
    
    ext_tsk();
}

/**
 * �e��^�X�N�����s��������n���h��
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
 * ���s�̂�Î~������^�X�N
 */
void ready_task(intptr_t exinf)
{
    gReadyToStart->run( TAIL_ANGLE_STAND_UP ); // �K�����~�낵�A���s�̂��Œ肷��
    if(bt_cmd == 1 || gTouchSensor.isPressed() == 1)
    {
        state = 1; // state �� 0 ���� 1 �ֈڍs
    }
}

// ���X��<end>

/**
 * ���C���g���[�X�^�X�N
 */
void tracer_task(intptr_t exinf)
{
    if ( ev3_button_is_pressed(BACK_BUTTON) ) {
        state = 2;  // �o�b�N�{�^������(state �� 1 ���� 2 �ֈڍs)
    } else {
        //tail_control(TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */
        gLineTracer->run();  // �|�����s
    }
    ext_tsk();
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
        uint8_t c = fgetc(bt);  /* ��M */
        switch(c){
        case '1' :
            bt_cmd = 1;
            break;
        default:
            break;
        }
        fputc(c, bt); /* �G�R�[�o�b�N */
    }
}
