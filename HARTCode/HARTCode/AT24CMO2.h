#ifndef AT24CMO2
#define AT24CMO2

#include "TWIComm.h"
#include "USARTComm.h"
#include <string.h>

#define AT24 0b1010 << 4
#define W 0 << 0
#define R 1 << 0
#define CHIP0 0 << 3
#define CHIP1 1 << 3

#define ADDMSB 1

/* Add the two most significant bits of the R/W address to bits 1 and 2
262,144 words, 1024 pages of 256 bytes each
Byte address lowest 8 bits of address, upper 10 bits page
Writing to AT24CMO2: Send address with desired address word. Get ack, send next part of address.
Repeat. Send Datum. Stop.

NEED 12ms DELAY AFTER WRITE FOR INTERNAL WRITE CYCLE

To page write continue and don't send a stop, but can only work up to max of 256 bytes and then rolls over.
Reading from AT24CMO2: Send address with desired address word. Get ack, send next part of address.
Repeat. Send another start command with read set. Send acks after every received word to keep receiving.
NACK to finish and send stop. 

DOESN'T KEEP TRACK OF PAGE OVERFLOW*/

void AddressSetup(uint8_t wr, uint8_t* AddBytes, uint32_t Address, uint8_t chip);
void WriteStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize);
void MemWrite(uint32_t Address, uint8_t* Packet, uint8_t size, uint8_t chip);
void ReadStream(uint8_t* packet, uint8_t* addbytes, uint8_t psize, uint8_t addsize);
void MemRead(uint32_t Address, uint8_t* Readbyte, uint8_t size, uint8_t chip);
void SavePacket(uint32_t* Address, uint8_t* packet, uint8_t size);
void ReadEEPROM(void);

#endif