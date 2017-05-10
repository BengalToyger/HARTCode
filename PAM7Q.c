#include "PAM7Q.h"

//Use RATE (PUBX,40)
uint16_t InitGPS(uint8_t* GPS, char* packet){
	uint8_t volatile i;
	uint16_t volatile SetUBRR; //Turns off all the messages we don't want
	char CFGMSG[CFGMSGSIZE] = CFGMSGBASE;
	SetUBRR = InitUSART(GPSBAUD, GPSPORT);
	if (SetUBRR){
		do {
			PUBXCFGSetup(CFGMSG, "GGA");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(5);
			PUBXCFGSetup(CFGMSG, "GLL");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(5);
			PUBXCFGSetup(CFGMSG, "GSA");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(5);
			PUBXCFGSetup(CFGMSG, "GSV");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(5);
			PUBXCFGSetup(CFGMSG, "RMC");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(5);
			PUBXCFGSetup(CFGMSG, "VTG");
			SendGPS(CFGMSG, CFGMSGSIZE);
			_delay_ms(200);
			i++;
			//ADD TIMEOUT FOR IF NOT GPS
		} while (!PollPUBX00(packet) && i <= 20);
		initGPSTimer();
		if (i > 20){
			*GPS = 0;
		} else {
			*GPS = 1;
		}
		return SetUBRR;
	} else {
		return 0;
	}
}

void PUBXCFGSetup(char* packet, char* msg){
	uint8_t volatile i = MSGSTT; //Sets up the configure message to turn off all the messages we don't want.
	uint8_t volatile j = 0; //Takes the message name
	for (i; i < MSGEND; i++){
		packet[i] = msg[j];
		j++;
	}
	CheckSum(packet);
	return;
}

void SendGPS(char* poll, uint8_t size){
	uint8_t volatile i; //Sends all the bytes in a msg to the GPS. 
	USARTTX('$', GPSPORT); //Prefaces with $ and appends carriage return newline
	for (i = 0; i < size; i++){
		USARTTX(poll[i], GPSPORT);
	}
	USARTTX('\r', GPSPORT);
	USARTTX('\n', GPSPORT);
	return;
}

void ReadGPS(char* packet, uint8_t size){
	uint8_t volatile i = 0; //Collects all the GPS packet bytes
	uint8_t volatile j = 0;
	for (j = 0; j < 100; j++){
		if (USARTRX(GPSPORT) == '$'){
			for (i = 0; i < size; i++){
				packet[i] = USARTRX(GPSPORT);
			}
			packet[i] = '\0';
			return;
		}
	}
	packet[i] = '\0';
	return;
}

uint8_t PollPUBX00(char* packet){
	cli();
	USART0Flush();
	SendGPS(PUBX00POLL, PUBX00SIZE);
	ReadGPS(packet, RSIZE);
	sei();
	if (packet[0] == '\0'){
		return 0;
	} else if (!checkPUBX(packet)){
		return 0;	
	} else {
		return 1;
	}
}

void CheckSum(char* packet){
	uint8_t volatile i = 0;
	uint8_t volatile checksum;
	char hexchar[2];
	while(!(packet[i] == '*')){
		checksum ^= packet[i]; //XORs all the packet bytes together to get the checksum
		i++;
	}
	BintoHexChar(checksum, hexchar);
	i++;
	packet[i] = hexchar[0];
	i++;
	packet[i] = hexchar[1];
	return;
}

void BintoHexChar(uint8_t bin, char* hexchar){
	uint8_t volatile i; //Converts a binary number into two character hexadecimal
	uint8_t volatile conv;
	for (i = 0; i < 2; i++){
		conv = (bin >> (1-i)*4);
		conv &= 0x0F;
		if (conv == 0){
			hexchar[i] = '0';
		} else if (conv == 1){
			hexchar[i] = '1';
		} else if (conv == 2){
			hexchar[i] = '2';
		} else if (conv == 3){
			hexchar[i] = '3';
		} else if (conv == 4){
			hexchar[i] = '4';
		} else if (conv == 5){
			hexchar[i] = '5';
		} else if (conv == 6){
			hexchar[i] = '6';
		} else if (conv == 7){
			hexchar[i] = '7';
		} else if (conv == 8){
			hexchar[i] = '8';
		} else if (conv == 9){
			hexchar[i] = '9';
		} else if (conv == 10){
			hexchar[i] = 'A';
		} else if (conv == 11){
			hexchar[i] = 'B';
		} else if (conv == 12){
			hexchar[i] = 'C';
		} else if (conv == 13){
			hexchar[i] = 'D';
		} else if (conv == 14){
			hexchar[i] = 'E';
		} else if (conv == 15){
			hexchar[i] = 'F';
		}
	}
	return;
} 

void ParsePUBX(char* packet, struct GPSStruct* GPSdata){
	char* parsedata = "0000000000000";
	uint8_t volatile i = 8; //Starts at beginning of time field
	uint8_t volatile dcnt = 0;
	char volatile ns;
	char volatile ew;
	parsedata[12] = '\0';
	while (packet[i] != ','){ //Skips through time field looking for next comma
		i++;
		if (i > 100){
			break;
		}
	}
	i++;
	while (packet[i] != ','){
		if (i > 100){
			break;
		}
		parsedata[dcnt] = packet[i]; //Puts latitude into parse data string. Goes until next comma. If no latitude, should not enter as it sees a comma
		i++;
		dcnt++;
	}
	parsedata[dcnt] = '\0'; //Puts null char at end so can do atof. If field was empty parsedata is just a null char
	if (dcnt){
		GPSdata->latitude = getDegreesLat(parsedata);
	} else {
		GPSdata->latitude = 0; //Gives latitude a 0 if no data in field
	}
	i++;
	if (packet[i] != ',' && packet[i] <= 100){ //Looks for North/South indicator. Defaults North.
		ns = packet[i];
		i += 2;
	} else {
		ns = 'N';
		i++;
	}
	resetParsedata(parsedata);
	dcnt = 0;
	while (packet[i] != ','){ //Gets longitude data
		if (i > 100){
			break;
		}
		parsedata[dcnt] = packet[i];
		i++;
		dcnt++;
	}
	parsedata[dcnt] = '\0';
	if (dcnt){
		GPSdata->longitude = getDegreesLong(parsedata);
	} else {
		GPSdata->longitude = 0;
	}
	i++;
	if (packet[i] != ',' && i <= 100){ //Gets East/West. Defaults West
		ew = packet[i];
		i += 2;
	} else {
		ew = 'W'; 
		i++;
	}
	resetParsedata(parsedata);
	dcnt = 0;
	while (packet[i] != ','){ //Gets altitude data
		if (i > 100){
			break;
		}
		parsedata[dcnt] = packet[i];
		i++;
		dcnt++;
	}
	parsedata[dcnt] = '\0';
	if (dcnt){
		GPSdata->GPSAltitude = (float)atof(parsedata);	
	} else {
		GPSdata->GPSAltitude = 0;
	}
	
	if (ew == 'W'){ //Sets positive/negative based on E/W N/S
		GPSdata->longitude = -1*GPSdata->longitude;
	}
	if (ns == 'S'){
		GPSdata->latitude = -1*GPSdata->latitude;
	}
	return;
}

void GetLLA(struct GPSStruct* GPSdata, uint8_t en, char* packet){
	if (checkGPSTimer() && en){
		if (PollPUBX00(packet)){
			ParsePUBX(packet, GPSdata);
			return;
		} else {
			GPSdata->GPSAltitude = 0;
			GPSdata->latitude = 0;
			GPSdata->longitude = 0;
			return;
		}
	} else {
		GPSdata->GPSAltitude = 0;
		GPSdata->latitude = 0;
		GPSdata->longitude = 0;
		return;
	}
}

void resetParsedata(char* parsedata){
	uint8_t volatile dcnt;
	for (dcnt = 0; dcnt < 12; dcnt++){
		parsedata[dcnt] = '0';
	}
	return;
}

uint8_t checkPUBX(char* gpsPacket){
	if (gpsPacket[0] == 'P' && gpsPacket[1] == 'U' && gpsPacket[2] == 'B' && gpsPacket[3] == 'X'){
		return 1;
	}
	else {
		return 0;
	}
}

void initGPSTimer(void){
	TCCR3B = CSDIV256;
	TCNT3 = 0;
	return;
}

void resetGPSTimer(void){
	TCNT3 = 0;
	return;
}

uint8_t checkGPSTimer(void){
	uint16_t volatile chk;
	chk = TCNT3;
	if (chk > ONEP2SECCLKDIV256){
		resetGPSTimer();
		return 1;
	} else {
		return 0;
	}
}

float getDegreesLong(char* longitude){
	char degreesString[4];
	char minutesString[9];
	uint16_t volatile degrees;
	float volatile minutes;
	memcpy(degreesString,longitude,3);
	degreesString[3] = '\0';
	degrees = atoi(degreesString);
	memcpy(minutesString,longitude+3,8);
	degreesString[8] = '\0';
	minutes = atof(degreesString);
	minutes = minutes / 60;
	return (float)degrees + minutes;
}

float getDegreesLat(char* longitude){
	char degreesString[3];
	char minutesString[9];
	uint16_t volatile degrees;
	float volatile minutes;
	memcpy(degreesString,longitude,2);
	degreesString[2] = '\0';
	degrees = atoi(degreesString);
	memcpy(minutesString,longitude+2,8);
	degreesString[8] = '\0';
	minutes = atof(degreesString);
	minutes = minutes / 60;
	return (float)degrees + minutes;
}