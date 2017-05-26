#ifndef USARTCOMM
#define USARTCOMM

#include "cpufreq.h"
#include <avr/io.h>

uint16_t SetBR(uint32_t baud, uint8_t port);
uint16_t InitUSART(uint32_t baud, uint8_t port);
void USARTTX(uint8_t TXval, uint8_t port);
char USARTRX(uint8_t port);
void USART0Flush(void);

#endif