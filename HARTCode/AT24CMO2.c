#include "AT24CMO2.h"

void AddressSetup(uint8_t wr, uint8_t* AddBytes, uint32_t Address, uint8_t chip){
	uint8_t volatile i;
	for (i = 0; i < 3; i++){
		AddBytes[i] = 0;
	}
	AddBytes[0] = AT24 | chip | wr | ((Address >> (16 - ADDMSB)) & (3 << 1));
	AddBytes[1] = (Address >> 8) & 255;
	AddBytes[2] = Address & 255;
	return;
}

void WriteStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize){
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

void MemWrite(uint32_t Address, uint8_t* Packet, uint8_t size, uint8_t chip){
	uint8_t AddBytes[3];
	AddressSetup(W, AddBytes, Address, chip);
	WriteStream(Packet, AddBytes, size, 3);
	return;
}

void ReadStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize){
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

void MemRead(uint32_t Address, uint8_t* Readbyte, uint8_t size, uint8_t chip){
	uint8_t AddBytes[3];
	AddressSetup(W, AddBytes, Address, chip);
	ReadStream(Readbyte, AddBytes, size, 3);
	return;
}

void SavePacket(uint32_t* Address, uint8_t* packet, uint8_t size){
	uint8_t volatile ChipSel;
	uint16_t volatile BAddress;
	uint8_t nppacket[size];
	uint8_t volatile npsize;
	BAddress = *Address & (0x000000FF);
	if (*Address < 262144){
		ChipSel = CHIP0;
	} else {
		ChipSel = CHIP1;
	}
	if ((BAddress + size) < 255){
		MemWrite(*Address, packet, size, ChipSel);
		Address += size;
	} else {
		npsize = BAddress + size - 255;
		memcpy(nppacket, (uint8_t*)(packet + size - npsize), npsize);
		MemWrite(*Address, packet, size - npsize, ChipSel);
		Address += size - npsize;
		if (*Address < 262144){
			ChipSel = CHIP0;
			} else {
			ChipSel = CHIP1;
		}
		_delay_ms(11);
		MemWrite(*Address, nppacket, npsize, ChipSel);
		*Address += npsize;
	}
}

void ReadEEPROM(void){
	uint8_t readStream[256];
	uint16_t i;
	uint8_t j;
	uint8_t k;
	uint32_t address;
	for (j = 0; j < 2; j++){
		address = 0;
		for (i = 0; i < 1024; i++){
			MemRead(address, readStream, 6, j);
			address += 256;
			for (k = 0; k <= 254; k++){
				
			}
		}
	}
}
