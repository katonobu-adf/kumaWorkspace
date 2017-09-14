
#include "PID.h"

PID::PID(float kc, float tc) {
	PID::kp = (0.6 * kc);
	PID::ti = (0.5 * tc);
	PID::td = (0.125 * tc);
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
