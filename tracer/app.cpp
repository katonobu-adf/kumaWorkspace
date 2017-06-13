
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
 * �Z���T�[�A���[�^�[�̐ڑ����`���܂�
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

static int      bt_cmd = 0;     /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;     /* Bluetooth�t�@�C���n���h�� */

/* ���L�̃}�N���͌�/���ɍ��킹�ĕύX����K�v������܂� */
/* sample_c1�}�N�� */
#define GYRO_OFFSET  0          /* �W���C���Z���T�I�t�Z�b�g�l(�p���x0[deg/sec]��) */
#define LIGHT_WHITE  48         /* ���F�̌��Z���T�l */
#define LIGHT_BLACK  0          /* ���F�̌��Z���T�l */
/* sample_c2�}�N�� */
#define SONAR_ALERT_DISTANCE 30 /* �����g�Z���T�ɂ���Q�����m����[cm] */
/* sample_c3�}�N�� */
#define TAIL_ANGLE_STAND_UP  90 /* ���S��~���̊p�x[�x] */
#define TAIL_ANGLE_DRIVE      3 /* �o�����X���s���̊p�x[�x] */
#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          60 /* ���S��~�p���[�^����PWM��΍ő�l */
/* sample_c4�}�N�� */
//#define DEVICE_NAME     "ET0"  /* Bluetooth�� hrp2/target/ev3.h BLUETOOTH_LOCAL_NAME�Őݒ� */
//#define PASS_KEY        "1234" /* �p�X�L�[    hrp2/target/ev3.h BLUETOOTH_PIN_CODE�Őݒ� */
#define CMD_START         '1'    /* �����[�g�X�^�[�g�R�}���h */

/* LCD�t�H���g�T�C�Y */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

const double TRAGET_VAL = ((LIGHT_BLACK + LIGHT_WHITE) / 2);
const double INTERVAL   = 0.004;

static float prev_e_t = -1;

// double KP = 0.00;
// const double TI = 10000000.0;
// const double TD = 0;

const int FORWARD = 80;
const double KC = 2.50;
const double TC = 0.080;
const double KP = (0.6 * KC);
const double TI = (0.5 * TC);
const double TD = (0.125 * TC);

/* �֐��v���g�^�C�v�錾 */
int limiter(float num, float min, float max);
int limiter(int num, int min, int max);
int turner(int sensor_val);
int abs(int num);
static void tail_control(signed int angle);

#include "BalancerCpp.h"
#include "Vals.h"
Balancer balancer;
Vals vals;

int limiter(float num, float min, float max) {
    if (num > max) return max;
    if (num < min) return min;

    return (int)num;
}// end limiter

int limiter(int num, int min, int max) {
    if (num > max) return max;
    if (num < min) return min;

    return num;
}// end limiter

int turner(int sensor_val) {
    float e_t;
    float der_e_t;
    float int_e_t = 0;
    float u_t;
    int temp_turn;
    unsigned short int m_val;

    m_val = sensor_val;
    e_t = (float)(TRAGET_VAL - m_val);
    int_e_t = int_e_t + e_t * INTERVAL;
    der_e_t = (e_t - prev_e_t) / INTERVAL;

    u_t = KP * (e_t + int_e_t / TI + TD * der_e_t);
    temp_turn = (int)u_t;
    prev_e_t = e_t;

    return limiter(temp_turn, -100.0, 100.0);
}// end turner

int abs(int num) {
    if (num < 0)
        num *= -1;
    return num;
}// end abs

static void tail_control(signed int angle) {
    float pwm = (float)(angle - ev3_motor_get_counts(tail_motor))*P_GAIN; /* ��ᐧ�� */

    /* PWM�o�͖O�a���� */
    if (pwm > PWM_ABS_MAX) {
        pwm = PWM_ABS_MAX;
    } else if (pwm < -PWM_ABS_MAX) {
        pwm = -PWM_ABS_MAX;
    }// end if

    if (pwm == 0) {
        ev3_motor_stop(tail_motor, true);
    } else {
        ev3_motor_set_power(tail_motor, (signed char)pwm);
    }// end if
}// end tail_control

/* ���C���^�X�N */
void main_task(intptr_t unused) {

    /* LCD��ʕ\�� */
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    ev3_lcd_draw_string("EV3way-ET sample_c4", 0, CALIB_FONT_HEIGHT*1);

    /* �Z���T�[���̓|�[�g�̐ݒ� */
    ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);
    ev3_color_sensor_get_reflect(color_sensor); /* ���˗����[�h */
    ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
    ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
    /* ���[�^�[�o�̓|�[�g�̐ݒ� */
    ev3_motor_config(left_motor, LARGE_MOTOR);
    ev3_motor_config(right_motor, LARGE_MOTOR);
    ev3_motor_config(tail_motor, LARGE_MOTOR);
    ev3_motor_reset_counts(tail_motor);

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    /* Bluetooth�ʐM�^�X�N�̋N�� */
    act_tsk(BT_TASK);

    ev3_led_set_color(LED_ORANGE);

    while(1) {
        tail_control(TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */

        if (bt_cmd == 1)
            break; /* �����[�g�X�^�[�g */

        if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
            break; /* �^�b�`�Z���T�������ꂽ */

        tslp_tsk(10); /* 10msec�E�F�C�g */
    }// end while

    ev3_led_set_color(LED_GREEN);

    act_tsk(SONAR_TASK);
    act_tsk(GYRO_TASK);
    act_tsk(COLOR_TASK);
    act_tsk(MOTOR_TASK);
    act_tsk(VOLTAGE_TASK);

    while(1) {

        if (ev3_button_is_pressed(BACK_BUTTON)) break;
        if (vals.getStop()) break;

        tslp_tsk(20);
    }// end while

    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(SONAR_TASK);
    ter_tsk(GYRO_TASK);
    ter_tsk(COLOR_TASK);
    ter_tsk(MOTOR_TASK);
    ter_tsk(VOLTAGE_TASK);
    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}// end main_task

void sonar_task(intptr_t unused) {
    signed int distance;

    while (true) {
        distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);

        if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0)) {
            vals.setAlert(1);
        } else {
            vals.setAlert(0);
        }// end if

        tslp_tsk(40);
    }// end while
}// end sonar_task

void gyro_task(intptr_t unused) {
    /* �W���C���Z���T�[���Z�b�g */
    ev3_gyro_sensor_reset(gyro_sensor);
    balancer.init(GYRO_OFFSET);

    while (true) {
        vals.setGyro(ev3_gyro_sensor_get_rate(gyro_sensor));

        /* �|���U�q����API���Ăяo���A�|�����s���邽�߂� */
        /* ���E���[�^�o�͒l�𓾂� */
        balancer.setCommand(vals.getForward(), vals.getTurn());
        balancer.update(vals.getGyro(), vals.getMotor_ang_r(), vals.getMotor_ang_l(), vals.getVolt()); // <2>

        tslp_tsk(4);
    }// end while
}// end gyro_task

void color_task(intptr_t unused) {
    prev_e_t = (float)(TRAGET_VAL - ev3_color_sensor_get_reflect(color_sensor));

    while (true) {
        vals.setReflect(ev3_color_sensor_get_reflect(color_sensor));

        tslp_tsk(20);
    }// end while
}// end color_task

void motor_task(intptr_t unused) {
    signed char pwm_L, pwm_R; /* ���E���[�^PWM�o�� */

    /* ���s���[�^�[�G���R�[�_�[���Z�b�g */
    ev3_motor_reset_counts(left_motor);
    ev3_motor_reset_counts(right_motor);

    while (true) {
        tail_control(TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */

        if (vals.getAlert() == 1) { /* ��Q�����m */
            vals.setTurn(0);
            vals.setForward(0);
        } else {
            vals.setTurn(turner(vals.getReflect()));
            vals.setForward(FORWARD - limiter(abs(vals.getTurn()), 0, 60));
        }// end if

        /* �|���U�q����API �ɓn���p�����[�^���擾���� */
        vals.setMotor_ang_l(ev3_motor_get_counts(left_motor));
        vals.setMotor_ang_r(ev3_motor_get_counts(right_motor));

        pwm_L = balancer.getPwmRight();
        pwm_R = balancer.getPwmLeft();

        /* EV3�ł̓��[�^�[��~���̃u���[�L�ݒ肪���O�ɂł��Ȃ����� */
        /* �o��0���ɁA���̓s�x�ݒ肷�� */
        if (pwm_L == 0) {
             ev3_motor_stop(left_motor, true);
        } else {
            ev3_motor_set_power(left_motor, (int)pwm_L);
        }// end if

        if (pwm_R == 0) {
             ev3_motor_stop(right_motor, true);
        } else {
            ev3_motor_set_power(right_motor, (int)pwm_R);
        }// end if

        tslp_tsk(8);
    }// end while
}// end motor_task

void voltage_task(intptr_t unused) {
    while (true) {
        vals.setVolt(ev3_battery_voltage_mV());

        tslp_tsk(30000);
    }// end while
}// end voltage_task

void bt_task(intptr_t unused) {
    while(1) {
        uint8_t c = fgetc(bt); /* ��M */
        switch(c) {
            case '0':
                vals.setStop(true);
                break;
            case '1':
                bt_cmd = 1;
                break;
            default:
                break;
        }// end switch
        // fputc(c, bt); /* �G�R�[�o�b�N */
    }// end while
}// end bt_task
