#include "Firing.h"

void FIREENABLE(void){
	DDRC = 255;
	return;
}

void FIREDISABLE(void){
	DDRC = 0;
	return;
}

void fire2(void){
	FIREPORT |= (1 << FIRE2);
	return;
}

void fire8(void){
	FIREPORT |= (1 << FIRE8);
}

void reset2(void){
	FIREPORT &= ~(1 << FIRE2);
	return;
}

void reset8(void){
	FIREPORT &= ~(1 << FIRE8);
	return;
}

void fireDrogue1(void){
	FIREPORT |= (1 << DROGUE1);
	return;
}

void fireDrogue2(void){
	FIREPORT |= (1 << DROGUE2);
	return;
}

void fireMain1(void){
	FIREPORT |= (1 << MAIN1);
	return;
}

void fireMain2(void){
	FIREPORT |= (1 << MAIN2);
	return;
}

void fireSepIgnite1(void){
	FIREPORT |= (1 << SEPIGNITE1);
	return;
}

void fireSepIgnite2(void){
	FIREPORT |= (1 << SEPIGNITE2);
	return;
}

void resetDrogue1(void){
	FIREPORT &= ~(1 << DROGUE1);
	return;
}

void resetDrogue2(void){
	FIREPORT &= ~(1 << DROGUE2);
	return;
}

void resetMain1(void){
	FIREPORT &= ~(1 << MAIN1);
	return;
}

void resetMain2(void){
	FIREPORT &= ~(1 << MAIN2);
	return;
}

void resetSepIgnite1(void){
	FIREPORT &= ~(1 << SEPIGNITE1);
	return;
}

void resetSepIgnite2(void){
	FIREPORT &= ~(1 << SEPIGNITE2);
	return;
}

uint8_t readEmatch(void){
	return CHECKPIN;
}