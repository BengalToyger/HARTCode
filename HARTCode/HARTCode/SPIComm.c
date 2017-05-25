#include "SPIComm.h"

void SPISetMode(uint8_t mode){
	SPCR = ((SPCR & ~(3 << CPHA)) | mode);
	return;
}

void SPIMasterInit(void){
	/* Set MOSI and SCK output, all others input */
	DDRB = DDRB | (1<<MOSI)|(1<<SCK);
	PORTB = PORTB | (1<<PB0);
	/* Enable SPI, Master, set clock rate fclk/16 125kHz */
	SPCR = ((SPCR & ~((1<<SPE)|(1<<MSTR)|(1<<SPR1))) | ((1<<SPE)|(1<<MSTR)|(1<<SPR1)));
	return;
}

void SPIMasterTransmit(uint8_t datum){
	SPIMasterInit();
	/* Start transmission */
	SPDR = datum;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF))){};
	return;
}

uint8_t SPIMasterRead(void){
	SPDR = 0;
	while(!(SPSR & (1<<SPIF))){};
	return SPDR;
}

/* Reads in up to a 32 bit value */
uint32_t SPIRead32Bit(uint8_t NumBytes){
	uint32_t volatile ReadByte = 0;
	uint8_t volatile i = 0;
	for (i; i < NumBytes; i++){
		ReadByte = ReadByte << 8;
		ReadByte += SPIMasterRead();
	}
	return ReadByte;
}