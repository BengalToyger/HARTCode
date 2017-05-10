#include "USARTComm.h"

uint16_t SetBR(uint32_t baud, uint8_t port){
	uint16_t volatile SetUBRR;
	SetUBRR = (F_CPU/(16*baud) - 1);
	if (SetUBRR > 4095 || !SetUBRR){
		return 0;
	} else {
		if (port){
			UBRR1 = SetUBRR;
			return SetUBRR;
		} else {
			UBRR0 = SetUBRR;
			return SetUBRR;
		}
	}
}

uint16_t InitUSART(uint32_t baud, uint8_t port){
	uint16_t volatile SetUBRR;
	SetUBRR = SetBR(baud, port);
	if (SetUBRR){
		if (port){
			UCSR1B = (1 << RXEN1)|(1 << TXEN1)|(1 << RXCIE1);
			return SetUBRR;
		} else {
			UCSR0B = (1 << RXEN0)|(1 << TXEN1);
			return SetUBRR;
		}
	} else {
		return 0;
	}
}

void USARTTX(uint8_t TXval, uint8_t port){
	if (port){	
		while(!(UCSR1A & (1 << UDRE1))){}
		UDR1 = TXval;
	} else {
		while(!(UCSR0A & (1 << UDRE0))){}
		UDR0 = TXval;
	}
}

char USARTRX(uint8_t port){
	uint16_t volatile i;
	if (port){
		for (i = 0; i < 3000; i++){
			if (UCSR1A & (1 << RXC1)){
				return UDR1;
			}
		}
	} else {
		for (i = 0; i < 3000; i++){
			if (UCSR0A & (1 << RXC0)){
				return UDR0;
			}
		}
	}
	return 0;
}

void USART0Flush(void){
	uint8_t volatile flush;
	while (UCSR0A & (1 << RXC0)){
		flush = UDR0;
	}
	return;
}