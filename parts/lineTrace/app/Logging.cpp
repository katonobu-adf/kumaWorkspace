#include "Logging.h"

FILE* mbt = NULL;

Logging::Logging(FILE* bt)
	: mbt(bt) {;}// end Logging 

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

void Logging::send(char val) {
	char str[10];

	sprintf(str, "%s,", val);

	int i = 0;
	while (str[i])
		fputc(str[i++], mbt);
}// end send