
#ifndef   DRIVINGMODE_H_
#define   DRIVINGMODE_H_

class DrivingMode {
public:
    DrivingMode();                // コンストラクタ
    int  prepare();               // 準備段階
    int  setCommand(int command); // コマンドセット
    int  exec();                  // 実行
protected:
    int mSubState;    // 状態を表す
    int mCommand;
};

#endif    /* DRIVINGMODE_H_ */