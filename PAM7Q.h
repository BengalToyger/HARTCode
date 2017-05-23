#ifndef PAM7Q
#define PAM7Q

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "USARTComm.h"
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>

/* RESET I IN GPS FUNCTIONS AND USART */

//Talker ID: GP
#define GPSPORT 0
#define PUBX00POLL "PUBX,00*33" //WORKS!
#define PUBX00SIZE 10
#define RSIZE 49
#define GPSBAUD 9600
#define CFGMSGBASE "PUBX,40,___,0,0,0,0,0,0*__"
#define CFGMSGSIZE 26
#define MSGSTT 8
#define MSGEND 11
#define GPSPRSCL 1/256
#define CSDIV256 1 << CS12
#define ONEP2SECCLKDIV256 37500

/* USES TIMER/COUNTER 3*/

struct GPSStruct {
	uint16_t GPSAltitude;
	float latitude;
	float longitude;
};

void initGPSTimer(void);
void resetGPSTimer(void);
uint8_t checkGPSTimer(void);

void SendGPS(char* poll, uint8_t size);
void ReadGPS(char* packet, uint8_t size);
void ResetGPS(void);
uint8_t PollPUBX00(char* packet);
void CheckSum(char* packet);
void PUBXCFGSetup(char* packet, char* msg);
void BintoHexChar(uint8_t bin, char* hexchar);
void ParsePUBX(char* packet, struct GPSStruct* GPSdata);
void resetParsedata(char* parsedata);
uint8_t checkPUBX(char* gpsPacket);
float getDegreesLat(char* latitude);
float getDegreesLong(char* longitude);

/*Primary Functions*/
uint16_t InitGPS(void); //Returns value put in UBRR
void GetLLA(struct GPSStruct* GPSdata, uint8_t en, char* packet);

#endif