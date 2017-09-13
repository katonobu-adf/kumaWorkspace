
/******************************************************************************
 *  PID.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#include "PID.h"

const int KP_COE = 0.6;
const int TI_COE = 0.5;
const int TD_COE = 0.125;

PID::PID(float kc, float tc) {
	this->kp = (KP_COE * kc);
	this->ti = (TI_COE * tc);
	this->td = (TD_COE * tc);
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
