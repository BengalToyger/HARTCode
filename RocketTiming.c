#include "RocketTiming.h"

void initSecondCounter(void){
	TCCR4B = CSDIV256; //2.09715 seconds until roll-over
	TCNT4 = 0;
	return;
}

uint8_t cntHalfSeconds(void){
	uint16_t volatile chk;
	chk = TCNT4;
	if (chk >= HALFSECCLKDIV256){
		TCNT4 = 0;
		return 1;
		} else {
		return 0;
	}
}

void initSendCnt(void){
	TCCR5B = CSDIV64; //.524 seconds until rollover
	TCNT5 = 0;
	return;
}

uint8_t sendCnt(void){
	uint16_t volatile chk;
	chk = TCNT5;
	if (chk >= SENDRATE){
		TCNT5 = 0;
		return 1;
		} else {
		return 0;
	}
}

void initIgniteCnt(void){
	TCCR4B = CSDIV1024;
	TCNT4 = 0;
	return;
}

uint8_t igniteCnt(void){
	uint16_t volatile chk;
	chk = TCNT4;
	if (chk >= EIGHTSECCLKDIV1024){
		TCNT4 = 0;
		return 1;
		} else {
		return 0;
	}
}

uint8_t resetandSecCntSepIgnite(void){
	uint16_t volatile chk;
	chk = TCNT4;
	if (chk >= HALFSECCLKDIV256 && chk <= ONESECCLKDIV256){
		resetSepIgnite1();
		return 0;
		} else if (chk >= ONESECCLKDIV256 && chk <= ONEANDHALFSECCLKDIV256){
		resetSepIgnite1();
		fireSepIgnite2();
		return 0;
		} else if (chk >= ONEANDHALFSECCLKDIV256){
		resetSepIgnite2();
		return 1;
		} else {
		return 0;
	}
}

uint8_t resetandSecCntDrogue(void){
	uint16_t volatile chk;
	chk = TCNT4;
	if (chk >= HALFSECCLKDIV256 && chk <= ONESECCLKDIV256){
		resetDrogue1();
		return 0;
		} else if (chk >= ONESECCLKDIV256 && chk <= ONEANDHALFSECCLKDIV256){
		resetDrogue1();
		fireDrogue2();
		return 0;
		} else if (chk >= ONEANDHALFSECCLKDIV256){
		resetDrogue2();
		return 1;
		} else {
		return 0;
	}
}

uint8_t resetandSecCntMain(){
	uint16_t volatile chk;
	chk = TCNT4;
	if (chk >= HALFSECCLKDIV256 && chk <= ONESECCLKDIV256){
		resetMain1();
		return 0;
		} else if (chk >= ONESECCLKDIV256 && chk <= ONEANDHALFSECCLKDIV256){
		resetMain1();
		fireMain2();
		return 0;
		} else if (chk >= ONEANDHALFSECCLKDIV256){
		resetMain2();
		return 1;
		} else {
		return 0;
	}
}

void initSendCntGnd(void){
	TCCR5B = CSDIV256; //2.097 rollover cnt
	TCNT5 = 0;
	return;
}

uint8_t sendCntGnd(void){
	uint16_t volatile chk;
	chk = TCNT5;
	if (chk >= ONESECCLKDIV256){
		TCNT5 = 0;
		return 1;
		} else {
		return 0;
	}
}