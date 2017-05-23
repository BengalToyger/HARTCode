#include "Rocket.h"

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

void collectData(struct DataStruct* data){
	uint16_t volatile temp;
	int32_t PT[2];
	GetAccelGyroXYZ(&(data->AccelData));
	GetCPT(data->CC, PT);
	data->degreesC = PT[1];
	data->ematch = readEmatch();
	temp = CalcAltitudeConstTemp(PT);
	data->altitude = temp;
	if (data->state == LAUNCHPAD || data->state == PARACHUTE || data->mode == 4){
		GetLLA(&(data->GPSData),data->GPS,data->packet);
	}
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
	gpf = (float)data->matchSetReset;
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
	InitGPS(&(data->GPS), data->packet);
	InitAccel();
	InitAltimeter(data->CC);
	data->ematch = readEmatch();
	data->altitude = 0;
	data->groundLevel = 0;
	data->GPSData.GPSAltitude = 0;
	data->GPSData.latitude = 0;
	data->GPSData.longitude = 0;
	data->EstData.kVelocity = 0;
	data->matchSetReset = NONE;
	collectData(data);
	for (i = 0; i < 6; i++){
		collectData(data);
		data->groundLevel += data->altitude/6;
	}
	data->EstData.kAltitude = data->groundLevel;
	Initdt();
	rxFlag = 0;
	data->mode = 1;
	data->state = 0;
	sei();
	return;
}

void receiveArmed(struct DataStruct* data){
	if (rxFlag){
		if (rx[rxn-2] == 1 || rx[rxn-2] == 2){
			data->mode = rx[rxn-2];
			rxFlag = 0;	
		}
	}
	sei();
	return;
}

void launchPad(struct DataStruct* data){
	uint8_t payload[PAYLOAD2SIZE];
	initSendCnt();
	sei();
	data->mode = ARMED;
	do {
		//receiveArmed(data);
		collectData(data);
		Estimate(&(data->EstData),&(data->AccelData),data->altitude);
		if (data->mode == TESTING && sendCnt()){
			formPayloadMode1(data, payload);
			send(payload, PAYLOAD1SIZE, GNDMAC);
		} else if (data->mode == ARMED && sendCnt()){
			formPayloadMode2(data, payload);
			send(payload, PAYLOAD2SIZE, GNDMAC);
		}
	} while (data->AccelData.AccelXYZ[0] < BOOSTTRIGGER || data->mode == TESTING);
	cli();
	return;
}

void boost(struct DataStruct* data){
	data->state = BOOST;
	do {
		rocketMain(data);
	} while (data->AccelData.AccelXYZ[0] > STAGINGTRIGGER);
	FIREENABLE(); //Comment out, see if changes crapping out!
	return;
}

void stagingBooster(struct DataStruct* data){
	//FOR SECOND DROGUE:
	//TAKE REFERENCE KALMAN ALTITUDE
	//TWO SECONDS LATER TAKE ANOTHER altitude
	//SEE IF LOWER
	float volatile refAlt = 0;
	float volatile newAlt = 0;
	uint8_t volatile drogue1cnt = 0;
	uint8_t volatile drogue2cnt = 0;
	uint8_t volatile dd1 = 0;
	uint8_t volatile dd2 = 0;
	uint8_t volatile halfSecCnt = 0;
	data->state = STAGING;
	//Separate and reset
	fireSepIgnite1();
	initSecondCounter();
	data->matchSetReset = SEP1S;
	do {
		rocketMain(data);
		halfSecCnt += cntHalfSeconds();
		if (halfSecCnt >= 1 && halfSecCnt < 2){
			resetSepIgnite1();
			data->matchSetReset = SEP1R;
		} else if (halfSecCnt >= 2 && halfSecCnt < 3){
			resetSepIgnite1();
			fireSepIgnite2();
			data->matchSetReset = SEP2S;
		} else if (halfSecCnt >= 3){
			resetSepIgnite2();
			data->matchSetReset = SEP2R;
			dd1 = 1;
		}
	} while (!dd1);
	dd1 = 0;
	//Wait an amount of time to deploy drogue
	halfSecCnt = 0;
	initSecondCounter();
	do {
		rocketMain(data);
		halfSecCnt += cntHalfSeconds();
		//Times the number of seconds until drogue deployment after staging and ignites first drogue
		if (halfSecCnt >= DROGUETIMESECBOOSTER*2 && !drogue1cnt){ 
			fireDrogue1();
			data->matchSetReset = D1S;
			drogue1cnt = halfSecCnt;
			//Resets drogue1 after a half second
		} else if ((halfSecCnt >= (drogue1cnt + 1)) && !dd1 && drogue1cnt){ 
			resetDrogue1();
			data->matchSetReset = D1R;
			dd1 = 1;
		}
		//Every 2 seconds checks to see if booster is going down and fires drogue2
		if (!(halfSecCnt % 8) && halfSecCnt){ 
			newAlt = data->EstData.kAltitude;
			if ((newAlt < refAlt) && !dd2){
				fireDrogue2();
				data->matchSetReset = D2S;
				drogue2cnt = halfSecCnt;
			}
			refAlt = newAlt;
		}
		//Resets drogue2 after a half a second
		if ((halfSecCnt >= (drogue2cnt + 1)) && !dd2 && drogue2cnt){
			resetDrogue2();
			data->matchSetReset = D2R;
			dd2 = 1;
		}
	} while (!dd1 || !dd2);
	return;
}

void stagingSustainer(struct DataStruct* data){
	uint8_t volatile halfSecCnt = 0;
	uint8_t volatile dd1 = 0;
	data->state = STAGING;
	//Separate and reset
	initSecondCounter();
	do {
		rocketMain(data);
		halfSecCnt += cntHalfSeconds();
		if (halfSecCnt >= 8 && halfSecCnt < 9){
			fireSepIgnite1();
			data->matchSetReset = SEP1S;
		} else if (halfSecCnt >= 9 && halfSecCnt < 10){
			resetSepIgnite1();
			data->matchSetReset = SEP1R;
		} else if (halfSecCnt >= 10 && halfSecCnt < 11){
			fireSepIgnite2();
			data->matchSetReset = SEP2S;
		} else if (halfSecCnt >= 11){
			resetSepIgnite1();
			resetSepIgnite2();
			data->matchSetReset = SEP2R; 
			dd1 = 1;
		}
	} while (!dd1);
	
}

void sustainer (struct DataStruct* data){ 
	//FOR SECOND DROGUE:
	//TAKE REFERENCE KALMAN ALTITUDE
	//TWO SECONDS LATER TAKE ANOTHER altitude
	//SEE IF LOWER
	float volatile refAlt = 0;
	float volatile newAlt = 0;
	uint8_t volatile drogue1cnt = 0;
	uint8_t volatile drogue2cnt = 0;
	uint8_t volatile dd1 = 0;
	uint8_t volatile dd2 = 0;
	uint8_t volatile halfSecCnt = 0;
	data->state = SUSTAIN;
	initSecondCounter();
	do {
		rocketMain(data);
		halfSecCnt += cntHalfSeconds();
		if (halfSecCnt >= DROGUETIMESECSUSTAINER*2 && !drogue1cnt){
			fireDrogue1();
			data->matchSetReset = D1S;
			drogue1cnt = halfSecCnt;
		} else if (halfSecCnt >= drogue1cnt + 1 && !dd1){
			resetDrogue1();
			data->matchSetReset = D1R;
			dd1 = 1;
		}
		if (!(halfSecCnt % 8)){
			newAlt = data->EstData.kAltitude;
			if (newAlt < refAlt){
				fireDrogue2();
				data->matchSetReset = D2S;
				drogue2cnt = halfSecCnt;
			}
		}
		if (halfSecCnt >= drogue2cnt + 1 && !dd2){
			resetDrogue2();
			data->matchSetReset = D2R;
			dd2 = 1;
		}
	} while (!dd1 || !dd2);
	return;
}

void parachute(struct DataStruct* data){
	data->state = PARACHUTE;
	uint8_t volatile halfSecCnt = 0;
	uint8_t volatile dd1 = 0;
	//Wait until 
	do {
		rocketMain(data);
	} while ((data->altitude - data->groundLevel) > MAINALTITUDEAGL);
	//Deploy main chute and reset
	fireMain1();
	initSecondCounter();
	data->matchSetReset = M1S;
	do {
		rocketMain(data);
		halfSecCnt += cntHalfSeconds();
		if (halfSecCnt >= 1 && halfSecCnt < 2){
			resetMain1();
			data->matchSetReset = M1R;
		} else if (halfSecCnt >= 2 && halfSecCnt < 3){
			resetMain1();
			fireMain2();
			data->matchSetReset = M2S;
		} else if (halfSecCnt >= 3){
			resetMain1();
			data->matchSetReset = M2R;
			dd1 = 1;
		}
	} while (!dd1);
	return;
}

void ground(struct DataStruct* data){
	uint8_t payload[PAYLOAD2SIZE];
	data->state = GROUND;
	data->mode = LOWPOWER;
	sei();
	initSendCntGnd();
	do {
		receiveArmed(data);
		collectData(data);
		Estimate(&(data->EstData),&(data->AccelData),data->altitude);
		if (sendCntGnd()){
			formPayloadMode4(data, payload);
			send(payload, PAYLOAD4SIZE, GNDMAC);
		} 
	} while (1);
	return;
}

float angleFromVertical(struct DataStruct* data){
	float volatile degreesFromVertical = 0;
	float volatile magZYPlane = 0;
	float volatile totalMag = 0;
	magZYPlane = sqrtf(pow(data->AccelData.AccelXYZ[1],2)+pow(data->AccelData.AccelXYZ[2],2));
	totalMag = sqrtf(pow(magZYPlane,2)+pow(data->AccelData.AccelXYZ[0],2));
	if (magZYPlane/totalMag <= 1) {
		degreesFromVertical = 180*asin(magZYPlane/totalMag)/M_PI;
		return degreesFromVertical;
	} else {
		return 0;
	}
}

void testMain(void){
	struct DataStruct data;
	rocketInit(&data);
	while (1){
		launchPad(&data);
	}
}

void rocketMain(struct DataStruct* data){
	uint8_t payload[PAYLOAD2SIZE];
	collectData(data);
	Estimate(&(data->EstData),&(data->AccelData),data->altitude);
	if (sendCnt()){
		formPayloadMode2(data, payload);
		send(payload, PAYLOAD2SIZE, GNDMAC);
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

void sustainerMain(void){
	struct DataStruct data;
	rocketInit(&data);
	launchPad(&data);
	boost(&data);
	stagingSustainer(&data);
	sustainer(&data);
	parachute(&data);
	ground(&data);
}

void boosterMain(void){
	struct DataStruct data;
	rocketInit(&data);
	launchPad(&data);
	boost(&data);
	stagingBooster(&data);
	parachute(&data);
	ground(&data);
}