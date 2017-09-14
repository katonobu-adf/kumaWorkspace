/*
 *  タッチセンサー制御 サンプルプログラム（03_sample_touch）
 */

/*
 *  ターゲット依存の定義
 */
#include "target_test.h"

/*
 *  ターゲットに依存する可能性のある定数の定義
 */
#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY
extern void	main_task(intptr_t exinf);
extern void run_task(intptr_t exinf);
#endif /* TOPPERS_MACRO_ONLY */
