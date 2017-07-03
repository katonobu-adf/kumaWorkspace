/******************************************************************************
 *  app.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

/*
 *  �e�^�X�N�̗D��x�̒�`
 */
#define MAIN_PRIORITY    TMIN_APP_TPRI + 1  /* ���C���^�X�N�̗D��x */
#define MEASURE_PRIORITY  TMIN_APP_TPRI + 2

// ���R�ǉ� <begin>
#define BT_TASK_PRIORITY  TMIN_APP_TPRI + 3
// ���R�ǉ� <end>

/*
 *  �^�[�Q�b�g�Ɉˑ�����\���̂���萔�̒�`
 */
#ifndef STACK_SIZE
#define STACK_SIZE      4096        /* �^�X�N�̃X�^�b�N�T�C�Y */
#endif /* STACK_SIZE */

/*
 *  �֐��̃v���g�^�C�v�錾
 */
#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void measure_ambient_task(intptr_t exinf);
extern void measure_bright_task(intptr_t exinf);
extern void ev3_cyc_ambient(intptr_t exinf);
extern void ev3_cyc_bright(intptr_t exinf);
// ���R�ǉ� <begin>
extern void bt_task(intptr_t exinf);
// ���R�ǉ� <end>
#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
