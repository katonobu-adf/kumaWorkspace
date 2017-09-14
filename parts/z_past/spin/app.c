/*
 *  モーター制御　サンプルプログラム02_sample_motor
 */

#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* モーターのポートを設定 */
#define R_motor						EV3_PORT_B
#define L_motor						EV3_PORT_C

/* 待機時間の設定[msec] */
const uint32_t RUN_TIME_MS = 3000;

void run_task(intptr_t unused) {
	int degrees;
	int32_t speed_abs;
	bool_t blocking;
	static int32_t cnt = 0;
	char msg[100];

	// 回転角度をリセット
	ev3_motor_reset_counts(L_motor);

	// スピン
	ev3_motor_rotate(R_motor, 740, 50, false);
	ev3_motor_rotate(L_motor, -740, 50, false);
	tslp_tsk(RUN_TIME_MS);

	// 停止
	ev3_motor_stop(L_motor, true);
	ev3_motor_stop(R_motor, true);
	
}

void main_task(intptr_t unused) {
	/* モーターの設定 */
	ev3_motor_config( L_motor , LARGE_MOTOR );
	ev3_motor_config( R_motor , LARGE_MOTOR );

	/* タスクを開始する */
	act_tsk(RUN_TASK);
}
