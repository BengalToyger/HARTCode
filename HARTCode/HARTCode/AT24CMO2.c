#include "AT24CMO2.h"

void addressSetup(uint8_t wr, uint8_t* AddBytes, uint32_t Address, uint8_t chip){
	uint8_t volatile i;
	for (i = 0; i < 3; i++){
		AddBytes[i] = 0;
	}
	AddBytes[0] = AT24 | chip | wr | ((Address >> (16 - ADDMSB)) & (3 << 1));
	AddBytes[1] = (Address >> 8) & 255;
	AddBytes[2] = Address & 255;
	return;
}

void writeStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize){
	uint8_t volatile i;
	uint8_t volatile status;
	uint8_t volatile send;
	TWISendStart();
	TWIIntWait();
	status = TWSR;
	for (i = 0; i < addsize; i++){
		send = addbytes[i];
		TWISend(send);
		TWIIntWait();
		status = TWSR;
	}
	for (i = 0; i < psize; i++){
		TWISend(packet[i]);
		TWIIntWait();
		status = TWSR;
	}
	TWISendStop();
}

void memWrite(uint32_t Address, uint8_t* Packet, uint8_t size, uint8_t chip){
	uint8_t AddBytes[3];
	addressSetup(W, AddBytes, Address, chip);
	writeStream(Packet, AddBytes, size, 3);
	return;
}

void readStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize){
	uint8_t volatile i;
	uint8_t volatile status;
	TWISendStart();
	TWIIntWait();
	status = TWSR;
	for (i = 0; i < addsize; i++){
		TWISend(addbytes[i]);
		TWIIntWait();
		status = TWSR;
	}
	TWISendStart();
	TWIIntWait();
	status = TWSR;
	TWISend(addbytes[0] | R);
	TWIIntWait();
	status = TWSR;
	for (i = 0; i < psize - 1; i++){
		TWIACK();
		TWIIntWait();
		packet[i] = TWDR;
		status = TWSR;
		//TWIACK();
	}
	TWINACK();
	TWIIntWait();
	packet[i] = TWDR;
	status = TWSR;
	//TWINACK();
	TWISendStop();
	return;
}

void memRead(uint32_t Address, uint8_t* packet, uint8_t size, uint8_t chip){
	uint8_t AddBytes[3];
	addressSetup(W, AddBytes, Address, chip);
	readStream(packet, AddBytes, size, 3);
	return;
}

void savePacket(uint32_t* address, uint8_t* packet, uint8_t size){
	uint8_t volatile ChipSel;
	uint16_t volatile byteAddress;
	byteAddress = *address & (0x000000FF); //Get the byte address out of the overall address
	if (*address < 262144){ //Change chips if we're over
		ChipSel = CHIP0;
	} else if (*address < 524288) {
		ChipSel = CHIP1;
	} else {
		return;
	}
	if ((byteAddress + size) < 256){
		memWrite(*address, packet, size, ChipSel);
		*address += size;
	} else {
		*address += (256 - byteAddress);
		memWrite(*address, packet, size, ChipSel);
		*address += size;
	}
	return;
}

void readPacket(uint32_t* address, uint8_t* packet, uint8_t size){
	uint8_t volatile ChipSel;
	uint16_t volatile byteAddress;
	byteAddress = *address & (0x000000FF); //Get the byte address out of the overall address
	if (*address < 262144){ //Change chips if we're over
		ChipSel = CHIP0;
		} else if (*address < 524288) {
		ChipSel = CHIP1;
		} else {
		return;
	}
	if ((byteAddress + size) < 256){
		memRead(*address, packet, size, ChipSel);
		*address += size;
		} else {
		*address += (256 - byteAddress);
		memRead(*address, packet, size, ChipSel);
		*address += size;
	}
	return;
}
