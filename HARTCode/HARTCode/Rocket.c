#include "Rocket.h"

/*
uint8_t volatile rxn;
uint8_t volatile rx[256];
uint8_t volatile rxFlag;

ISR(USART1_RX_vect){
	uint16_t volatile i;
	cli();
	rxn = 0;
	for (i = 0; i < 500; i++){
		if (UCSR1A & (1<< RXC1)){
			rx[rxn++] = UDR1;
			i = 0;
		}
	} 
	rxFlag=1; // notify main of receipt of data.
}
*/

void collectData(struct DataStruct* data){
	uint16_t volatile temp;
	int32_t PT[2];
	GetAccelGyroXYZ(&(data->AccelData));
	GetCPT(data->CC, PT);
	data->degreesC = PT[1];
	data->ematch = readEmatch();
	temp = CalcAltitudeConstTemp(PT);
	data->altitude = temp;
	getGPSData(&(data->GPSData));
	return;
}

void formPayloadMode1(struct DataStruct* data, uint8_t* payload){
	uint8_t volatile n = 0;
	payload[n] = 1;
	n++;
	payload[n] = data->matchSetReset;
	n++;
	payload[n] = data->degreesC;
	return;
}

void formPayloadMode2(struct DataStruct* data, uint8_t* payload){
	uint16_t volatile gpb = 0;
	uint8_t volatile n = 0;
	float volatile gpf = 0;
	payload[n] = 2; //0
	n += 1;
	payload[n] = 0; //1st
	n += 1;
	gpb = round(data->altitude);
	memcpy(payload+n,&gpb,2); //2-3
	n += 2;
	memcpy(payload+n,&(data->GPSData.latitude),4); //4-7
	n += 4;
	memcpy(payload+n,&(data->GPSData.longitude),4); //8-11
	n += 4;
	memcpy(payload+n,data->AccelData.AccelXYZ,4); //12-15
	n += 4;
	memcpy(payload+n,data->AccelData.AccelXYZ+1,4); //16-19
	n += 4;
	memcpy(payload+n,data->AccelData.AccelXYZ+2,4); //20-23
	n += 4;
	memcpy(payload+n,data->AccelData.GyroXYZ,4); //24-27
	n += 4;
	memcpy(payload+n,data->AccelData.GyroXYZ+1,4); //28-31
	n += 4;
	memcpy(payload+n,data->AccelData.GyroXYZ+2,4); //32-35
	n += 4;
	gpf = (float)data->state;
	memcpy(payload+n,&gpf,4); //36-39
	n += 4;
	gpf = (float)data->degreesC;
	memcpy(payload+n,&gpf,4); //40-43
	n += 4;
	gpf = 0;
	memcpy(payload+n,&gpf,4); //44-47
	n += 4;
	gpb = round(data->GPSData.GPSAltitude);
	memcpy(payload+n,&gpb,2); //48-49
	n += 2;
	gpb = round(data->EstData.kAltitude); 
	memcpy(payload+n,&gpb,2); //50-51
	n += 2;
	gpb = round(data->EstData.kVelocity);
	memcpy(payload+n,&gpb,2); //52-53
	n += 2;
	payload[n] = data->matchSetReset; //54
	return;
}

void formPayloadMode4(struct DataStruct* data, uint8_t* payload){
	uint8_t volatile n = 0;
	payload[n] = 4;
	n++;
	memcpy(payload+n,&(data->GPSData.latitude),4); //4-7
	n += 4;
	memcpy(payload+n,&(data->GPSData.longitude),4); //8-11
	return;
}

void rocketInit(struct DataStruct* data){
	uint8_t volatile i;
	_delay_ms(100);
	InitUSART(RADIOBAUD, RADIOPORT);
	InitGPS();
	InitAccel();
	InitAltimeter(data->CC);
	data->ematch = readEmatch();
	data->altitude = 0;
	data->groundLevel = 0;
	data->refAlt = 0;
	data->newAlt = 0;
	data->halfSecCnt = 0;
	data->GPSData.GPSAltitude = 0;
	data->GPSData.latitude = 0;
	data->GPSData.longitude = 0;
	data->EstData.kVelocity = 0;
	data->eepromAddress = 0;
	data->matchSetReset = NONE;
	data->mode = ARMED;
	data->state = LAUNCHPAD;
	for (i = 0; i < 6; i++){
		collectData(data);
		data->groundLevel += data->altitude/6;
	}
	data->EstData.kAltitude = data->groundLevel;
	Initdt();
	initSendCnt();
	sei();
	return;
}

void rocketMain(struct DataStruct* data){
	uint8_t payload[PAYLOAD2SIZE];
	collectData(data);
	Estimate(&(data->EstData),&(data->AccelData),data->altitude);
	if (sendCnt()){
		formPayloadMode2(data, payload);
		send(payload, PAYLOAD2SIZE, GNDMAC);
		if (data->state >= BOOST && data->state < GROUND){
			savePacket(&(data->eepromAddress), payload, PAYLOAD2SIZE);
		}
	}
	return;
}

void stateOutput(struct DataStruct* data){
	if (data->state == STAGING){
		data->halfSecCnt += cntHalfSeconds();
		#ifdef SUSTAINER
		if (data->halfSecCnt >= 16){
			data->matchSetReset = SEP1S;
		}
		#else
		data->matchSetReset = SEP1S;
		#endif
		if (!(data->halfSecCnt % 8) && data->halfSecCnt){
			data->refAlt = data->newAlt;
			data->newAlt = data->altitude;
		}
	}
	return;
}

void nextState(struct DataStruct* data){
	if (data->state == LAUNCHPAD && data->AccelData.AccelXYZ[0] > BOOSTTRIGGER){
		data->state = BOOST;
	} else if (data->state == BOOST && data->AccelData.AccelXYZ[0] < STAGINGTRIGGER){
		data->state = STAGING;
		initSecondCounter();
	} else if (data->state == STAGING && data->matchSetReset == SEP1S && data->newAlt < data->refAlt){
		data->state = PARACHUTE;
	} else if (data->state == PARACHUTE && (data->altitude - data->groundLevel) < MAINALTITUDEAGL){
		data->state = GROUND;
	}
	return;
}

void dataOnlyMain(void){
	struct DataStruct data;
	rocketInit(&data);
	while (1){
		rocketMain(&data);
	}
}

void stateMain(void){
	struct DataStruct data;
	rocketInit(&data);
	while (1){
		rocketMain(&data);
		nextState(&data);
		stateOutput(&data);
	}
}

void readEEPROMData(void){
	uint8_t payload[PAYLOAD2SIZE];
	struct DataStruct data;
	rocketInit(&data);
	while(1){
		readPacket(&(data->eepromAddress), payload, PAYLOAD2SIZE);
		send(payload, PAYLOAD2SIZE, GNDMAC);
	}
}