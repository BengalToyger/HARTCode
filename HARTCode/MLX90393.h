/* MLX9039.h is the include file for the MLX9039.h magnetometer */

#ifndef MLX90393
#define MLX90393

#include "ChipSelect.h"
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "SPIComm.h"

/* ZYXT bit shift selects */
#define ZS 3
#define YS 2
#define XS 1
#define TS 0

/* Magnetometer Commands */
#define MAGBURSTMODE 0x10 //Turns on burst mode
#define MAGREAD 0x50 //Reads a reg
#define MAGWRITE 0x60 // Writes to a reg
#define MAGREADM 0x40 | 1 << ZS | 1 << YS | 1 << ZS | 0 << TS // Reads the magnetometer measurements
#define MAGRESET 0xF0

/* Magnetometer reg spaces and addresses */
#define MGAIN 0x00 //Reg for gain
#define MBURSTSEL 0x01 //Reg for burst settings
#define MRES 0x02 //Reg for resolution settings

/* Bit shifts */
/* MGAIN */
#define BIST 0 //Built in self test, applies Z-Field
#define MGAINS 4 //Measurement gain settings
/* MBURSTSEL */
#define COMMMODE 5 //Communication mode
#define BURSTZY 0 //Selects measurements being used
#define BURSTXT 6
#define BDR 0 //Burst data rate
/* MRES */
#define FILT 2
#define OSR 0

/* Values */
#define SPIONLY 2
#define BURSTZYS 3
#define BURSTXTS 2 
#define BDRS 1 //20 ms datarate
#define FILTS 2
#define OSR 0
#define OSRS 2

/* High and low byte settings */
#define MGAINSH 0 << BIST //No coil test
#define MGAINSL 0 << MGAINS | 0x0C //0 Gain
#define MBURSTMSH SPIONLY << COMMMODE | BURSTZY << BURSTZYS  //Only SPI, ZY meas set
#define MBURSTMSL BURSTXT << BURSTXTS | BDR << BDRS
#define MRESSH 0b00001000
#define MRESSL FILT << FILTS | OSR << OSRS

/* Gain conversion values */
#define XYG0R0 0.805
#define ZG0R0 1.468

void MagSelect(void);
void MagRelease(void);
void MagReset(void);
void WriteMagReg(uint8_t, uint8_t, uint8_t);
void MagBurstMode(void);
void ReadMagMeasurement(int16_t*);

/* Primary functions */
void InitMag(void); //Run at initilization
void GetConvMagMeasurement(float*); //Want space for 3 floats

#endif


