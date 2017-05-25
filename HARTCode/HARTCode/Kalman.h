#ifndef KALMAN
#define KALMAN

#ifndef F_CPU 
#define F_CPU 8000000UL
#endif

#include "MPU6000.h"
#include <math.h>

#define K11 0.6668
#define K21 0.0327

#define KALMANPRSCL 1/64
#define CSDIV64 (0 << CS12)|(1 << CS11)|(1 << CS10)

/* USES TIMER/COUNTER 1*/

struct KalmanStruct {
	float kAltitude;
	float kVelocity;
};

void Initdt(void);
void Resetdt(void);
float Readdt(void);

void Estimate(struct KalmanStruct* estimate, struct AccelStruct* accelData, float altitude);

#endif