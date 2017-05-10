#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "radio_ctl.h"

#define F_CPU			8000000

//change mac address from a #define int to an actual 8 byte number,called by send
void macDecode(int mac, unsigned char *macAddr)
{
	unsigned char sustainerMAC[8] = {0x00,0x13,0xA2,0x00,0x41,0x5A,0xD2,0x0B};
	unsigned char rdfMAC[8] =		{0x00,0x13,0xA2,0x00,0x41,0x26,0x47,0x61};
	unsigned char boosterMAC[8] =	{0x00,0x13,0xA2,0x00,0x41,0x25,0xD1,0xF6};
	unsigned char groundMAC[8] =	{0x00,0x13,0xA2,0x00,0x41,0x26,0x47,0x58}; //>>>>>>>>>>>>>add ground mac address
	
	
	switch(mac){
		case 1:
			memcpy(macAddr,sustainerMAC,8); //sustainer
			break;
		case 2:
			memcpy(macAddr,rdfMAC,8); //booster
			break;
		case 3:
			memcpy(macAddr,boosterMAC,8); //rdf
			break;
		case 4:
			memcpy(macAddr,groundMAC,8); //ground
			break;	
	}
}



void initUART1()
{
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);   // Turn on the transmission and reception circuitry and enable rx interupt
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11); // Use 8-bit character sizes

	UBRR1H = 0x00; // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 9600 baud
	UBRR1L = 0x33; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register 9600 baud
	
		
	
}


void send(unsigned char *msg, int len, int macAddr){
	
	unsigned char buf[256];
	unsigned char escBuff[256];
	int escapedLen = 0;
	unsigned char checksum = 0;
	unsigned char addr[8];
	
	macDecode(macAddr,addr); //call to change #define int to 8 byte address

	buf[0] = 0x7E;
	buf[1] = 0x00;
	// LSB = content + 5 (content length + API type + frameid + addr(2) + options)
	buf[2] = (unsigned char)(len + 11);
	buf[3] = 0x00;  // frame type - 64bit tx
	buf[4] = 0x00;  // Frame ID
	for (int j = 0;j<8;j++)
	{
		buf[j+5] = addr[j];
	}
	buf[13] = 0x01;  // Disable acknowledge>>>>>>>>>>may need to reenable for rdf,auto responce gives rssi?
	memcpy(&buf[14], msg, len);

	for (int i=3;i<len+14;i++){
		checksum += buf[i];
	}

	// Total length = LSB + 9 (LSB value + MSB + LSB + start delimiter + checksum)
	buf[len+14] = 0xFF - checksum;
	escapedLen = escape(buf, len+15, escBuff);
	
	//send data out serial port
	for (int i = 0; i<escapedLen;i++)
	{
		while (!(UCSR1A & (1 << UDRE1))) {};
		UDR1 = escBuff[i];
	}
}

int receive(unsigned char *inBuff, int len, unsigned char *outBuff){
	int unescapeLen = 0;
	//unsigned char checksum = 0;
	unsigned char LSB = 0;

	if (inBuff[0] != 0x7E)
	return 0;

	if (len < 16)
	return 0;

	unescapeLen = unescape(inBuff, len, outBuff);

	// Check we have at least the amount of bytes indicated by LSB
	LSB = outBuff[2];
	if (LSB > (unescapeLen - 4))
	return 0;

	/* >>>>>>>>>>>>>>>>>>>may not need as radios dont pass packets with bad checksums
	// Calculate our checksum
	// (char will overflow, no need to AND for lower bytes)
	for (int i=3; i<LSB+4; i++){
		checksum += outBuff[i];
	}

	if (checksum != 0xFF)
	return 0;
	*/
	return LSB+4;
}

int escape(unsigned char *input, int inLen, unsigned char *output){
	int pos = 1;

	output[0] = input[0];
	for (int i=1; i<inLen; i++){
		switch(input[i]){
			case 0x7D:
			case 0x7E:
			case 0x11:
			case 0x13:
			output[pos++] = 0x7D;
			output[pos++] = input[i] ^ 0x20;
			break;
			default:
			output[pos++] = input[i];
			break;
		}
	}

	return pos;
}

int unescape(unsigned char *input, int inLen, unsigned char *output){
	int pos = 1;
	bool skip = false;
	unsigned char curr = 0;

	output[0] = input[0];
	for (int i=1; i<inLen; i++) {
		if (skip){
			skip = false;
			continue;
		}

		if (input[i] == 0x7D){
			curr = input[i+1] ^ 0x20;
			skip = true;
			}else{
			curr = input[i];
		}

		output[pos] = curr;
		pos++;
	}

	return pos;
}