
/******************************************************************************
 *  PID.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

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
