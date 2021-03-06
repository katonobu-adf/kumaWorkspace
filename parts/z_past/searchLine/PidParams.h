
#ifndef    PIDPARAMS_H_
#define    PIDPARAMS_H_

#include   "picojson.h"


class PidParams {

private:
    int id;
   	float    KC;             // ÀE´x@Éæé±Uo®ÌäáQC
	float    TC;	        // ÀE´x@Éæé±U®Ìüú
	float    KPx;           // äá®ìÌäáW
	float    TIx;           // Ïª®ìÌäáW
	float	 TDx;         // ÷ª®ìÌäáW

public:
    static bool loadParamsFromFile(const char * fileName);
    PidParams();
    PidParams getPid();
};

#endif     /* PIDPARAMS_H_ */