
#ifndef    PIDPARAMS_H_
#define    PIDPARAMS_H_

#include   "picojson.h"


class PidParams {

private:
    int id;
   	float    KC;             // ŒÀŠEŠ´“x–@‚É‚æ‚é‘±Uo“®‚Ì”ä—áƒQƒCƒ“
	float    TC;	        // ŒÀŠEŠ´“x–@‚É‚æ‚é‘±U“®‚ÌüŠú
	float    KPx;           // ”ä—á“®ì‚Ì”ä—áŒW”
	float    TIx;           // Ï•ª“®ì‚Ì”ä—áŒW”
	float	 TDx;         // ”÷•ª“®ì‚Ì”ä—áŒW”

public:
    static bool loadParamsFromFile(const char * fileName);
    PidParams();
    PidParams getPid();
};

#endif     /* PIDPARAMS_H_ */