#ifndef FIRING
#define FIRING

#include "cpufreq.h"
#include <avr/io.h>
#include "USARTComm.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define FIREPORT PORTC
#define CHECKPIN PINA

#define CHK1 PA5
#define CHK2 PA4 //Broken on booster
#define CHK3 PA0
#define CHK4 PA1
#define CHK5 PA2
#define CHK6 PA3
#define CHK7 PA6
#define CHK8 PA7 //Broken on booster

#define FIRE1 PC7
#define FIRE2 PC6 //CHK broken, don't use
#define FIRE3 PC3
#define FIRE4 PC2
#define FIRE5 PC5
#define FIRE6 PC4
#define FIRE7 PC1
#define FIRE8 PC0 //CHK broken, don't use

#define SEPIGNITE1 FIRE1
#define SEPIGNITE2 FIRE3
#define DROGUE1 FIRE4
#define DROGUE2 FIRE5
#define MAIN1 FIRE6
#define MAIN2 FIRE7

/* USES TIMER/COUNTER 4 (Not at the same time as rocket main) */

/* SUSTAINER CONT CHECK HEX 
		7D
*/

/* BOOSTER CONT CHECK HEX
		6D
*/


void FIREENABLE(void);
void FIREDISABLE(void);

void fire2(void);
void fire8(void);

void reset2(void);
void reset8(void);

void fireDrogue1(void);
void fireDrogue2(void);
void fireMain1(void);
void fireMain2(void);
void fireSepIgnite1(void);
void fireSepIgnite2(void);

void resetDrogue1(void);
void resetDrogue2(void);
void resetMain1(void);
void resetMain2(void);
void resetSepIgnite1(void);
void resetSepIgnite2(void);

uint8_t readEmatch(void);

#endif