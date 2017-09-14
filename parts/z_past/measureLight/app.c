/*
 *  タッチセンサー制御 サンプルプログラム  (03_sample_touch)
 */

#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* センサーのポートを設定 */
#define touch_sensor			EV3_PORT_1
#define color_sensor			EV3_PORT_3

/* 待機時間の設定 [msec] */
const uint32_t WAIT_TIME_MS = 50;

char *modeString[] = {
	"ambient\n",
	"reflect\n",
};
char msgBuf[128];
FILE *fp;
static int toggle=0;
void run_task(intptr_t unused) {

	static bool_t	touch_pressed;
	uint8_t         reflect;
	uint8_t         ambient;

	fp=fopen("log.txt","a");
	if ( fp == NULL ) return;
	while(1){
		touch_pressed = ev3_touch_sensor_is_pressed(touch_sensor);
		if(touch_pressed){
			if (toggle==0){
				ambient=ev3_color_sensor_get_ambient(color_sensor);
				sprintf(msgBuf,"ambient:%3d, ", ambient );
				fputs(msgBuf, fp);
				ev3_lcd_draw_string(msgBuf, 10, 30);
			}
			else
			{
				reflect=ev3_color_sensor_get_reflect(color_sensor);
				sprintf(msgBuf,"reflect:%3d, ", reflect );
				fputs(msgBuf, fp);
				ev3_lcd_draw_string(msgBuf, 10, 30);
			}
		} else {
		}
		fflush(fp);
		tslp_tsk(WAIT_TIME_MS);
	}
	fclose(fp);
}

static void button_clicked_handler( intptr_t button ){
	switch(button){
		case LEFT_BUTTON:
			toggle = 0;
			ev3_lcd_draw_string(modeString[toggle], 10, 10);
			break;

		case RIGHT_BUTTON:
			toggle = 1;
			ev3_lcd_draw_string(modeString[toggle], 10, 10);
			break;

		case DOWN_BUTTON:
			// 終了
			ev3_lcd_draw_string("The End of Task!!!!!!!!", 10, 10);
			ev3_lcd_draw_string("Push The BACK Button!!!", 10, 30);
			break;
			fclose(fp);
			ter_tsk(RUN_TASK);
			break;
			
		default:
			break;
	}
}

void main_task(intptr_t unused) {

	ev3_button_set_on_clicked( LEFT_BUTTON,  button_clicked_handler, LEFT_BUTTON);
	ev3_button_set_on_clicked( RIGHT_BUTTON, button_clicked_handler, RIGHT_BUTTON);
	ev3_button_set_on_clicked( DOWN_BUTTON,  button_clicked_handler, DOWN_BUTTON);
	
	/* センサーの設定 */
	ev3_sensor_config( color_sensor , COLOR_SENSOR );
	ev3_sensor_config( touch_sensor , TOUCH_SENSOR );

    /* タスクを開始する */
    act_tsk(RUN_TASK);
}
