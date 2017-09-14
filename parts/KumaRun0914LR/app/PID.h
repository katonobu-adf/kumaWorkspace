
/******************************************************************************
 *  PID.h (for ET Robocon 2017 Tohoku)
 *  Created on: 2017/08/05
 *  Author: Ryota Suzuki
 *  Copyright (c) 2017 ABC/ADK
 *****************************************************************************/

#ifndef PID_H
#define PID_H

class PID {

public:
	PID(float kc, float tc);
	float getKP();
	float getTI();
	float getTD();

private:
    float kp;
    float ti;
    float td;

};

#endif