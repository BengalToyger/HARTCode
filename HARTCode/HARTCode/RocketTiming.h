#ifndef TIMING
#define TIMING

#include "Rocket.h"
#include "Firing.h"

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define CSDIV64 (0 << CS12)|(1 << CS11)|(1 << CS10) //.524 sec to rollover
#define CSDIV256 1 << CS12 //2.097 sec to rollover
#define CSDIV1024 (1 << CS12) | (1 << CS10) //8.388 sec to rollover

#define P4SECCLKDIV64 0.4*F_CPU/64
#define P1SECCLKDIV64 0.1*F_CPU/64
#define P05SECCLKDIV64 0.05*F_CPU/64
#define ONESECCLKDIV256 F_CPU/256
#define ONEANDHALFSECCLKDIV256 3*F_CPU/(256*2)
#define HALFSECCLKDIV256 F_CPU/(256*2)
#define EIGHTSECCLKDIV1024 8*F_CPU/1024

#define SENDRATE P1SECCLKDIV64

/**
 * \brief Inits TCNT4 to clock divided by 256. Has a rollover time of 
 * slightly over 2 seconds with F_CPU 8000000
 * \param void
 * 
 * \return void
 */
void initSecondCounter(void);



/**
 * \brief If TCNT4 is at 1/2 second or above at 8000000 F_CPU,
 * returns 1 and resets. Otherwise returns 0;
 * \param 
 * 
 * \return uint8_t
 */
uint8_t cntHalfSeconds(void);



//////////////////////////////////////////////////////////////////////////
//Inits TCNT5 to clock divided by 64. Has a rollover time of slightly over
//half a second with F_CPU 8000000
//////////////////////////////////////////////////////////////////////////

void initSendCnt(void);



//////////////////////////////////////////////////////////////////////////
//Checks to see if TCNT5 is over the send rate (Currently 10 Hz). If it 
//is, resets counter and returns a 1. Else returns a 0.
//////////////////////////////////////////////////////////////////////////

uint8_t sendCnt(void);



//////////////////////////////////////////////////////////////////////////
//Inits TCNT4 to clock divided by 1024. Has a rollover time of slightly
//over 8 seconds
//////////////////////////////////////////////////////////////////////////

void initIgniteCnt(void);



//////////////////////////////////////////////////////////////////////////
//If TCNT4 has counted to 8 seconds at clock divided by 1024, returns 1 
//and resets. Else, returns 0.
//////////////////////////////////////////////////////////////////////////

uint8_t igniteCnt(void);


//////////////////////////////////////////////////////////////////////////
//If TCNT4 is between .5 and 1 seconds at clock divided by 256, 
//resets SepIgnite/Drogue/Main fire first fire pin. 
//If TCNT4 is between 1 and .5, fires the second fire pin. if TCNT4
//is after 1.5 seconds, resets the second fire pin.
//////////////////////////////////////////////////////////////////////////

uint8_t resetandSecCntSepIgnite(void);
uint8_t resetandSecCntDrogue(void);
uint8_t resetandSecCntMain(void);



//////////////////////////////////////////////////////////////////////////

void initSendCntGnd(void);

uint8_t sendCntGnd(void);

#endif