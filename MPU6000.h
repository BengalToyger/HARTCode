/* MPU6000.h is the header file for the accelerometer */

#ifndef MPU6000
#define MPU6000
 
#include "ChipSelect.h"
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "SPIComm.h"

#define ACCELCONFIG 0x1C
#define GYROCONFIG 0x1B
#define USERCTRL 0x6A
#define PWRMNGMNT1 0x6B
#define SIGNALRESET 0x68

#define AFS2G 0 << 3
#define AFS16G 3 << 3
#define FS1000DEG 2 << 3
#define I2CIFDIS 1 << 4
#define RESET 1 << 7
#define SRESET 7

#define GYROCON 1/32.8
#define ACCELCON -9.8/2048 

#define ACCELREAD 1 << 7
#define ACCELWRITE ~(1 << 7)

#define ACCXOUTH 0x3B //Acceleration along X axis
#define ACCXOUTL 0x3C
#define ACCYOUTH 0x3D
#define ACCYOUTL 0x3E
#define ACCZOUTH 0x3F
#define ACCZOUTL 0x40

#define GYROXOUTH 0x43 //Rotation around X axis
#define GYROXOUTL 0x44
#define GYROYOUTH 0x45
#define GYROYOUTL 0x46
#define GYROZOUTH 0x47
#define GYROZOUTL 0x48

struct AccelStruct {
	float AccelXYZ[3];
	float GyroXYZ[3];
};

void AccelSelect(void);
void AccelRelease(void);
void WriteAccelerometer(uint8_t, uint8_t);
uint8_t ReadAccelerometer(uint8_t);
int16_t AccReadZ(void);
int16_t AccReadY(void);
int16_t AccReadX(void);
int16_t GyroReadZ(void);
int16_t GyroReadY(void);
int16_t GyroReadX(void);
void ResetAccelerometer(void);

/* Primary Functions */
void InitAccel(void); // Use at initialization
void GetAccelGyroXYZ(struct AccelStruct* AccelData);

#endif