
/******************************************************************************
 *  app.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki, Ken Sasaki, Kouhei Okuyama
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

/*
 *  各タスクの優先度の定義
 */
#define MAIN_PRIORITY     TMIN_APP_TPRI + 1  /* メインタスクの優先度 */
#define RACER_PRIORITY    TMIN_APP_TPRI + 2
#define READY_PRIORITY    TMIN_APP_TPRI + 3
#define BT_TASK_PRIORITY  TMIN_APP_TPRI + 4

/*
 *  ターゲットに依存する可能性のある定数の定義
 */
#ifndef STACK_SIZE
#define STACK_SIZE      6144        /* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY
extern void main_task(intptr_t exinf);
extern void ev3_cyc_task_runer(intptr_t exinf);
/*
 *　ev3_cyc_task_runer内で実行されるタスク
 */
extern void ready_task(intptr_t exinf);
extern void racer_task(intptr_t exinf);
extern void bt_task(intptr_t exinf);

// 奥山追加 <end>
#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
