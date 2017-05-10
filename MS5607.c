#include "MS5607.h"

void AltSelect(void){
	SPISetMode(MODE0);
	PORTB = PORTB | (1<<CSACCEL);
	PORTD = PORTD | (1<<CSMAG);
	PORTB = PORTB & ~(1<<CSALT);
	return;
}

void AltRelease(void){
	PORTB = PORTB | (1<<CSALT);
	return;
}

void AltReset(void){
	AltSelect();
	SPIMasterTransmit(ALTRESET);
	_delay_ms(3);
	AltRelease();
	return;
}

uint16_t AltReadProm(uint8_t PromAddr){
	uint16_t volatile ReadByte;
	AltSelect();
	SPIMasterTransmit(PromAddr);
	ReadByte = SPIRead32Bit(2);
	AltRelease();
	return ReadByte;
}

void AltReadCalCoeff(uint16_t* CalCoeff){
	uint8_t volatile PromOffset = 1;
	for (PromOffset; PromOffset < 8; PromOffset++){
		CalCoeff[PromOffset] = AltReadProm(ALTPROM + (PromOffset << 1));
	}
	return;
}

void InitAltimeter(uint16_t* CalCoeff){
	DDRB = DDRB | (1<<CSALT);
	AltReset();
	AltReadCalCoeff(CalCoeff);
	return;
}

void InitTempConv(void){
	AltSelect();
	SPIMasterTransmit(ALTFASTTEMPCONV);
	AltRelease();
	_delay_us(1000);
	return;
}

void InitPresConv(void){
	AltSelect();
	SPIMasterTransmit(ALTFASTPRESCONV);
	AltRelease();
	_delay_us(1000);
	return;
}

uint32_t ReadAltimeter(void){
	uint32_t volatile datum;
	AltSelect();
	SPIMasterTransmit(ALTADCREAD);
	datum = SPIRead32Bit(3);
	AltRelease();
	return datum;
}

void CalcPT(uint32_t* PT, uint16_t* CalCoeff, int32_t* CPT){
	int32_t volatile dT;
	int64_t volatile OFF;
	int64_t volatile SENS;
	dT=PT[1]-CalCoeff[5]*pow(2,8);
	OFF=CalCoeff[2]*pow(2,17)+dT*CalCoeff[4]/pow(2,6);
	SENS=CalCoeff[1]*pow(2,16)+dT*CalCoeff[3]/pow(2,7);

	CPT[1]=(2000+(dT*CalCoeff[6])/pow(2,23))/100;
	CPT[0]=(((PT[0]*SENS)/pow(2,21)-OFF)/pow(2,15))/100;
	return;
}

void GetCPT(uint16_t* CalCoeff, int32_t* CPT){
	uint32_t PT[2];
	InitPresConv();
	PT[0] = ReadAltimeter();
	InitTempConv();
	PT[1] = ReadAltimeter();
	CalcPT(PT, CalCoeff, CPT);
	return;
}

float CalcAltitudeConstTemp(int32_t* CPT){
	float volatile Altitude;
	float volatile TempK;
	float volatile Coeff;
	double volatile PressRatio;
	double volatile Pressure;
	Pressure = CPT[0];
	TempK = CPT[1] + 273.15;
	Coeff = 287.058*TempK/9.8;
	PressRatio = Pressure/1013;
	Altitude = -log(PressRatio)*Coeff;
	return Altitude;
}

