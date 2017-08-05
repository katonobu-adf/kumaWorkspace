
#include "PID.h"

PID::PID(float kc, float tc) {
	this->kp = (0.6 * kc);
	this->ti = (0.5 * tc);
	this->td = (0.125 * tc);
}// end PID

float PID::getKP() {
	return kp;
}// end getKP

float PID::getTI() {
	return ti;
}// end getTI

float PID::getTD() {
	return td;
}// end getTD
