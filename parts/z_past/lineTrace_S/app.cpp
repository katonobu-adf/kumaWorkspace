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
#include "Navigator.h"

// �f�X�g���N�^���̉��
void *__dso_handle=0;   // https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping

// using�錾
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::Motor;
using ev3api::TouchSensor;

//static int      bt_cmd = 0;     /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;     /* Bluetooth�t�@�C���n���h�� */

// ���X��<begin>
//int gState = 0;            /* Driver ��Ŏ��s����^�X�N��߂�ϐ� */
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

// ���X�؁A���R begin
static Navigator       *gNavigator;
static Driver          *gDriver;
// ���X�؁A���R end

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

    gNavigator       = new Navigator(gLineMonitor, 
                                     gBalancingWalker,
                                     gTouchSensor);
    
    gDriver          = new Driver( gNavigator,
                                   gLineMonitor, 
                                   gBalancingWalker,
                                   gTail);

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

    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;
    delete gNavigator;
    delete gDriver;

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
    
    act_tsk(RUN_TASK);
    slp_tsk(); // �X���[�v

    ev3_sta_cyc(EV3_CYC_TASK_RUNER); // �����n���h���J�n
    slp_tsk(); // �X���[�v

    ev3_stp_cyc(EV3_CYC_TASK_RUNER); // �����n���h���I��
    user_system_destroy();  // �I������
    
    ext_tsk();
}

/**
 * �e��^�X�N����s��������n���h��
 */
void ev3_cyc_task_runer(intptr_t)
{
    act_tsk(TRACER_TASK);
}

// ���X��<end>
/**
 * ���C���g���[�X�^�X�N
 */
void tracer_task(intptr_t exinf)
{
    if ( ev3_button_is_pressed(BACK_BUTTON) ) {
        wup_tsk(MAIN_TASK);  // �o�b�N�{�^������
    } else {
        gDriver->run();
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
            gNavigator->setBtCommand(1);
            break;
        default:
            break;
        }
        if(gNavigator->isOutputRequest()){
            fputs(gNavigator->getMsg(), bt); /* �G�R�[�o�b�N */
        }
    }
}

/* �ҋ@���Ԃ̐ݒ� [msec] */
const uint32_t WAIT_TIME_MS = 50;
static char msgBuf[128];
static int toggle=0;
static bool isBlack = true; // ���ˌ��𑪂�Ƃ��ɍ��𑪂邩�A���𑪂邩

// static int black = 0; // ���̔��ˌ�
// static int white = 0; // ���̔��ˌ�

void run_task(intptr_t unused) {

	bool	        touch_pressed;
	uint8_t         reflect;
	uint8_t         ambient;

	while(1){
        if(ev3_button_is_pressed(LEFT_BUTTON)){
            toggle = 0;
            ev3_lcd_draw_string("ambient trace", 10, 0);
        }
        if(ev3_button_is_pressed(RIGHT_BUTTON)){
            toggle = 1;
            ev3_lcd_draw_string("reflect trace", 10, 0);
            
            if(isBlack){
                ev3_lcd_draw_string("selected:Black", 10, 15);
            }else{
                ev3_lcd_draw_string("selected:White", 10, 15);
            }
        }
        if(ev3_button_is_pressed(UP_BUTTON)){
            isBlack = !isBlack;
            
            if(isBlack){
                ev3_lcd_draw_string("selected:Black", 10, 15);
            }else{
                ev3_lcd_draw_string("selected:White", 10, 15);
            }
        }
        touch_pressed = gTouchSensor.isPressed();
		if(touch_pressed){
			if (toggle==0){
				ambient=gColorSensor.getAmbient();
				sprintf(msgBuf,"ambient:%3d, ", ambient );
				ev3_lcd_draw_string(msgBuf, 10, 30);
			}
			else if(toggle==1)
			{
				reflect=gColorSensor.getBrightness();
				sprintf(msgBuf,"reflect:%3d, ", reflect );
				ev3_lcd_draw_string(msgBuf, 10, 30);

                /* �������̒l��i�[ */
                if(isBlack){
                    gLineMonitor->setCalibrateBlack(reflect);
                }else{
                    gLineMonitor->setCalibrateWhite(reflect);
                }
			}else{    
            }

		} else {
		}

        if(ev3_button_is_pressed(DOWN_BUTTON)){
            break;
        }

		tslp_tsk(WAIT_TIME_MS);
	}
    wup_tsk(MAIN_TASK);
}
