/* ChipSelect.h details the chip select pins for the sensors */

#ifndef CHIPSELECTH
#define CHIPSELECTH

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define CSMAG PD7 //Magnetometer Chip Select
#define CSACCEL PB5
#define CSALT PB6

#endif