/* MS5607.h is the include file for the altimeter. */

#ifndef MS5607_H
#define MS5607_H

#include "cpufreq.h"
#include "ChipSelect.h"
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "SPIComm.h"

#define ALTRESET 0x1E
#define ALTPROM 0xA0
#define ALTFASTPRESCONV 0x40
#define ALTFASTTEMPCONV 0x50
#define ALTADCREAD 0x00

#define SPRESSMBAR 1013
#define STEMPK 288.16
#define GNAUGHT 9.8

void AltSelect(void);
void AltRelease(void);
void AltReset(void);
uint16_t AltReadProm(uint8_t);
void InitTempConv(void);
void InitPresConv(void);
uint32_t ReadAltimeter(void);
void CalcPT(uint32_t*, uint16_t*, int32_t*);
void AltReadCalCoeff(uint16_t*);
/* Primary Functions, in this order */
void InitAltimeter(uint16_t*); //Use at initialization
void GetCPT(uint16_t*, int32_t*); //First variable is for calculation coefficients, second is for pressure and temp
float CalcAltitudeConstTemp(int32_t*); //Put in pressure and temp array

#endif