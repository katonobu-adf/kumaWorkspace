
#include "PID.h"

PID::PID(float KC, float TC) {
	PID::KP = (0.6 * KC);
	PID::TI = (0.5 * TC);
	PID::TD = (0.125 * TC);
}// end PID
