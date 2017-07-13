#ifndef STDIO_H
	#include <stdio.h>
#endif

#ifndef EV3API_H
	#include "ev3api.h"
#endif

class Logging {

public:
	Logging();

	void send(int val);
	void send(float val);
	// void send(char val);

private:
	static FILE* mbt;

};// end class