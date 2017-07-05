
#include "ReadyToStart.h"

#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */

/*
 * コンストラクタ
 * @param tail      転倒防止
 */
ReadyToStart::ReadyToStart(const ev3api::Motor* tail)
    :   mTail(tail),
        mIsInitialized(false) {

}

/*
 * 初期化設定
 */
void ReadyToStart::init(){
    mTail.setBrake(true); //ブレーキモードの設定
    mTail.reset(); // モータ内の角度リセット
    
    ReadyToStart::mIsInitialized = true;
}

/*
 * 尻尾を降ろす
 * 引数の angle で指定した角度で尻尾を固定する
 */
 void ReadyToStart::run(signed int angle){
     if(ReadyToStart::mIsInitialized == false){
         ReadyToStart->init();
     }

     float pwm = (float)(angle - mTail.getCount() )*P_GAIN; /* 比例制御 */
    /* PWM出力飽和処理 */
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
        mTail.stop();
    }
    else
    {
        mTail.setPWM((int)pwm);
    }
 }