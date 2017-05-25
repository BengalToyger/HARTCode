#include "MLX90393.h"

void MagSelect(void){
	SPISetMode(MODE3);
	PORTB = PORTB | (1<<CSACCEL);
	PORTD = PORTD & ~(1<<CSMAG);
	PORTB = PORTB | (1<<CSALT);
	return;
}

void MagRelease(void){
	PORTD = PORTD | (1<<CSMAG);
	return;
}

void MagReset(void){
	uint8_t volatile status;
	MagSelect();
	SPIMasterTransmit(MAGRESET);
	status = SPIRead32Bit(1);
	MagRelease();
	return;
}

void WriteMagReg(uint8_t Address, uint8_t HVal, uint8_t LVal){
	MagSelect();
	SPIMasterTransmit(MAGWRITE);
	SPIMasterTransmit(HVal);
	SPIMasterTransmit(LVal);
	SPIMasterTransmit(Address);
	MagRelease();
	return;
}

void InitMag(void){
	DDRD = DDRD | (1<<CSMAG);
	MagReset();
	_delay_ms(1);
	WriteMagReg(MGAIN << 2, MGAINSH, MGAINSL);
	WriteMagReg(MBURSTSEL << 2, MBURSTMSH, MBURSTMSL);
	WriteMagReg(MRES << 2, MRESSH, MRESSL);
	MagBurstMode();
	return;
}

void MagBurstMode(void){
	MagSelect();
	SPIMasterTransmit(MAGBURSTMODE|1 << ZS|1 << YS|1 << XS);
	MagRelease();
}

void ReadMagMeasurement(int16_t* MagXYZ){
	uint8_t volatile i = 0;
	uint8_t volatile status;
	MagSelect();
	SPIMasterTransmit(MAGREADM);
	status = SPIRead32Bit(1);
	for(i; i < 3; i++){
		MagXYZ[i] = SPIRead32Bit(2);
	}
	MagRelease();
	return;
}

void GetConvMagMeasurement(float* ConvMagXYZ){
	int16_t MagXYZ[3];
	ReadMagMeasurement(MagXYZ);
	ConvMagXYZ[0] = XYG0R0*MagXYZ[0];
	ConvMagXYZ[1] = XYG0R0*MagXYZ[1];
	ConvMagXYZ[2] = ZG0R0*MagXYZ[2];
	return;
}