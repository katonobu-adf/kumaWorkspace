
/******************************************************************************
 *  Logging.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef  LOGGING_H
#define  LOGGING_H

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
	void send(char * pStr);

private:
	static FILE* mbt;

};// end class

#endif   /* LOGGING_H */
