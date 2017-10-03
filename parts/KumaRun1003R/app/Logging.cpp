
/******************************************************************************
 *  Logging.cpp (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "Logging.h"

FILE* Logging::mbt = ev3_serial_open_file(EV3_SERIAL_BT);

Logging::Logging() {}// end Logging 

void Logging::send(int val) {
	char str[10];

	sprintf(str, "%d,", val);

	int i = 0;
	while (str[i])
		fputc(str[i++], mbt);
}// end send

void Logging::send(float val) {
	char str[10];

	sprintf(str, "%.2f,", val);

	int i = 0;
	while (str[i])
		fputc(str[i++], mbt);
}// end send

void Logging::send(char *pStr) {

	int i = 0;
	while (pStr[i])
		fputc(pStr[i++], mbt);
}// end send