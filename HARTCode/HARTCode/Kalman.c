#include "Kalman.h"

void Initdt(void){
	TCCR1B = CSDIV64;
	TCNT1 = 0;
	return;
}

void Resetdt(void){
	TCNT1 = 0;
	return;
}

float Readdt(void){
	float cnt;
	cnt = TCNT1;
	Resetdt();
	return cnt/((float)F_CPU*(float)KALMANPRSCL);
}

void Estimate(struct KalmanStruct* estimate, struct AccelStruct* accelData, float altitude){
	struct KalmanStruct propogated;
	float dt;
	dt = Readdt();
	propogated.kAltitude = altitude+dt*estimate->kVelocity+(1/2)*(accelData->AccelXYZ[0] - 9.8)*pow(dt,2);
	propogated.kVelocity = estimate->kVelocity+dt*(accelData->AccelXYZ[0] - 9.8);
	estimate->kAltitude = propogated.kAltitude + K11*(altitude - propogated.kAltitude);
	estimate->kVelocity = propogated.kVelocity + K21*(altitude - propogated.kAltitude);
	return;
}
