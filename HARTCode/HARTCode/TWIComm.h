#ifndef TWICOMM_H
#define TWICOMM_H

#include <avr/io.h>
#include <util/delay.h>

#define TWIFREQ 100000

uint8_t TWIinit(uint32_t twihz, uint32_t cpuclk);
void TWISendStart(void);
void TWIIntWait(void);
void TWISend(uint8_t);
void TWISendStop(void);
void TWIACK(void);
void TWINACK(void);

#endif