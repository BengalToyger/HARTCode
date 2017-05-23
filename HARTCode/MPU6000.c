#include "MPU6000.h"

void AccelSelect(void){
	SPISetMode(MODE0);
	PORTB = PORTB & ~(1<<CSACCEL);
	PORTD = PORTD | (1<<CSMAG);
	PORTB = PORTB | (1<<CSALT);
	return;
}

void AccelRelease(void){
	PORTB = PORTB | (1<<CSACCEL);
	return;
}

void WriteAccelerometer(uint8_t Address, uint8_t WriteVal){
	uint8_t volatile WAddress = Address & ACCELWRITE;
	AccelSelect();
	SPIMasterTransmit(WAddress);
	SPIMasterTransmit(WriteVal);
	AccelRelease();
	return;
}

uint8_t ReadAccelerometer(uint8_t Address){
	uint8_t volatile RAddress = Address | ACCELREAD;
	uint8_t volatile datum;
	AccelSelect();
	SPIMasterTransmit(RAddress);
	datum = SPIRead32Bit(1);
	AccelRelease();
	return datum;
}

void InitAccel(void){
	uint8_t volatile val;
	DDRB = DDRB | (1<<CSACCEL);
	ResetAccelerometer();
	_delay_ms(1);
	WriteAccelerometer(USERCTRL,I2CIFDIS);
	WriteAccelerometer(PWRMNGMNT1,0x00);
	val = ReadAccelerometer(0x75);
	val = val >> 1;
	_delay_ms(10);
	WriteAccelerometer(ACCELCONFIG,AFS16G);
	_delay_ms(10);
	WriteAccelerometer(GYROCONFIG,FS1000DEG);
	return;
}

int16_t AccReadZ(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(ACCZOUTH);
	ValL = ReadAccelerometer(ACCZOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

int16_t AccReadY(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(ACCYOUTH);
	ValL = ReadAccelerometer(ACCYOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

int16_t AccReadX(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(ACCXOUTH);
	ValL = ReadAccelerometer(ACCXOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

int16_t GyroReadZ(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(GYROZOUTH);
	ValL = ReadAccelerometer(GYROZOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

int16_t GyroReadY(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(GYROYOUTH);
	ValL = ReadAccelerometer(GYROYOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

int16_t GyroReadX(void){
	uint16_t volatile ValH = 0;
	uint16_t volatile ValL = 0;
	uint16_t volatile Val = 0;
	ValH = ReadAccelerometer(GYROXOUTH);
	ValL = ReadAccelerometer(GYROXOUTL);
	Val = (ValH << 8) | ValL;
	return Val;
}

void GetAccelGyroXYZ(struct AccelStruct* AccelData){
	uint8_t volatile i = 0;
	AccelData->AccelXYZ[0] = AccReadX();
	AccelData->AccelXYZ[1] = AccReadY();
	AccelData->AccelXYZ[2] = AccReadZ();
	AccelData->GyroXYZ[0] = GyroReadX();
	AccelData->GyroXYZ[1] = GyroReadY();
	AccelData->GyroXYZ[2] = GyroReadZ();
	for (i; i<3; i++){
		AccelData->AccelXYZ[i] = ACCELCON*AccelData->AccelXYZ[i];
		AccelData->GyroXYZ[i] = GYROCON*AccelData->GyroXYZ[i];
	}
	return;
}

void ResetAccelerometer(void){
	WriteAccelerometer(PWRMNGMNT1,RESET);
	_delay_ms(10);
	WriteAccelerometer(SIGNALRESET,SRESET);
	_delay_ms(10);
	WriteAccelerometer(SIGNALRESET,0);
	return;
}