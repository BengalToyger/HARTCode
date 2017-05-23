#include "TWIComm.h"

uint8_t TWIinit(uint32_t twihz, uint32_t cpuclk){
	uint32_t volatile TWBRset;
	TWBRset = (cpuclk - 16*twihz)/(2*twihz);
	if (TWBRset > 255 || !TWBRset){
		return 0;
	} else {
		TWBR = TWBRset;
		return TWBRset;
	}
}

void TWISendStart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	return;
}

/* TWIIntWait waits for start to send, for data to send and acks to be received before continuing.
 after sending anything, always wait. */

void TWIIntWait(void){
	while (!(TWCR & (1<<TWINT)));
	return;
}

/* Use to send data and addresses. */

void TWISend(uint8_t datum){
	TWDR = datum;
	TWCR = (1<<TWINT) | (1<<TWEN);
	return;
}

/* Use to send stop condition and relinquish control of the line. */

void TWISendStop(void){
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	return;
}

void TWIACK(void){
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	return;
}

void TWINACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
	return;
}