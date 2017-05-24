#ifndef ROCKET
#define ROCKET

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "RocketTiming.h"
#include "MPU6000.h"
#include "AT24CMO2.h"
#include "USARTComm.h"
#include "radio_ctl.h"
#include "PAM7Q.h"
#include "MS5607.h"
#include "Firing.h"
#include "Kalman.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MAGNETOMETER 0
#define RADIOBAUD 38400
#define RADIOPORT 1
#define PAYLOAD1SIZE 3
#define PAYLOAD2SIZE 55
#define PAYLOAD4SIZE 9

#define TESTING 1
#define ARMED 2
#define LOWPOWER 4

#define BOOSTTRIGGER 40
#define STAGINGTRIGGER -15

#define DROGUETIMESECSUSTAINER 11
#define DROGUETIMESECBOOSTER 21

#define MAINALTITUDEAGL 300

#define SUSTAINERMAC 1
#define RDFMAC 2
#define BOOSTERMAC 3
#define GNDMAC 4

/* STATES */

#define LAUNCHPAD 0
#define BOOST 1
#define STAGING 2
#define SUSTAIN 3
#define PARACHUTE 4
#define GROUND 5

/* Events */

#define NONE 0
#define SEP1S 1
#define SEP1R 2
#define SEP2S 3
#define SEP2R 4
#define D1S 5
#define D1R 6
#define D2S 7
#define D2R 8
#define M1S 9
#define M1R 10
#define M2S 11
#define M2R 12

#define UNITTEST

/* USES TIMER/COUNTER 4*/

struct DataStruct {
	struct AccelStruct AccelData;
	struct GPSStruct GPSData;
	struct KalmanStruct EstData;
	float altitude;
	float groundLevel;
	uint8_t GPS;
	uint8_t state;
	uint8_t degreesC;
	uint8_t mode;
	uint8_t matchSetReset;
	uint8_t ematch;
	uint8_t sensors;
	uint16_t CC[7];
	char packet[RSIZE+1];
};

void collectData(struct DataStruct* data);

void formPayloadMode1(struct DataStruct* data, uint8_t* payload);
void formPayloadMode2(struct DataStruct* data, uint8_t* payload);
void formPayloadMode4(struct DataStruct* data, uint8_t* payload);

void receiveArmed(struct DataStruct* data);

void rocketInit(struct DataStruct* data);

void launchPad(struct DataStruct* data);

void boost(struct DataStruct* data);

void stagingBooster(struct DataStruct* data);

void stagingSustainer(struct DataStruct* data);

void sustainer(struct DataStruct* data);

void parachute(struct DataStruct* data);

void ground(struct DataStruct* data);

float angleFromVertical(struct DataStruct* data);

void testMain(void);

void rocketMain(struct DataStruct* data);

void sustainerMain(void);
void boosterMain(void);

#endif
