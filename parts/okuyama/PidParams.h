
#ifndef    PIDPARAMS_H_
#define    PIDPARAMS_H_

#include   "picojson.h"


class PidParams {

private:
    int id;
   	float    KC;             // ���E���x�@�ɂ�鎝���Uo�����̔��Q�C��
	float    TC;	        // ���E���x�@�ɂ�鎝���U���̎���
	float    KPx;           // ��ᓮ��̔��W��
	float    TIx;           // �ϕ�����̔��W��
	float	 TDx;         // ��������̔��W��

public:
    static bool loadParamsFromFile(const char * fileName);
    PidParams();
    PidParams getPid();
};

#endif     /* PIDPARAMS_H_ */