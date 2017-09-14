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