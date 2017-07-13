#ifndef STDIO_H
	#include <stdio.h>
#endif

class Logging {

public:
	Logging(FILE* bt);

	void send(int val);
	void send(float val);
	void send(char val);

private:
	FILE* mbt;

};// end class