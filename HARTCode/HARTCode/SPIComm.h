/* SPIComm has functions for using SPI easily */

#ifndef SPICOMM_H
#define SPICOMM_H

#include "cpufreq.h"
#include <avr/io.h>

#define MOSI PB2
#define SCK PB1
#define MODE0 0 << CPHA
#define MODE3 3 << CPHA

void SPISetMode(uint8_t mode);
void SPIMasterInit(void);
void SPIMasterTransmit(uint8_t);
uint8_t SPIMasterRead(void);
uint32_t SPIRead32Bit(uint8_t);

#endif