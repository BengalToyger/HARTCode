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

// Parses the latitude, longitude, and altitude out of a GGA (interrupt) message
// Parameters:
//		packet:		the GGA message string
//		GPSdata:	the struct that accepts the final calculated data
// Returns:
//		Nothing
void parseGGA(char *packet, struct GPSStruct *GPSdata) {
	char *packetCopy = strdup(packet);
	// We are going to alter the packetCopy pointer with strsep, so keep
	// a copy of the original location so we can free() it later.
	char *originalPacketCopy = packetCopy; 
	// The string token that we are currently looking at
	char *msgPart = packetCopy;
	int i;
	
	// Skip the xxGGA and time fields
	for(i = 0; i < 2; i++) {
		strsep(&packetCopy, ",");
	}
	
	// get the latitude
	msgPart = strsep(&packetCopy, ",");
	GPSdata->latitude = parseDegreesMinutes(msgPart, 2);
	// get the N/S component of the latitude. If it's 'S', then make the latitude negative
	msgPart = strsep(&packetCopy, ",");
	if(*msgPart == 'S') {
		GPSdata->latitude = -GPSdata->latitude;
	}
	
	// get the longitude
	msgPart = strsep(&packetCopy, ",");
	GPSdata->longitude = parseDegreesMinutes(msgPart, 3);
	// get the E/W component of the longitude. If it's 'W', then make the longitude negative
	msgPart = strsep(&packetCopy, ",");
	if(*msgPart == 'S') {
		GPSdata->longitude = -GPSdata->longitude;
	}
	
	// Skip the quality, numSV, and HDOP fields
	for(i = 0; i < 3; i++) {
		strsep(&packetCopy, ",");
	}
	
	// Get the altitude. If there is no altitude, then set it to zero.
	msgPart = strsep(&packetCopy, ",");
	if(*msgPart != '\0') {
		GPSdata->GPSaltitude = atof(msgPart);
	} else {
		GPSdata->GPSaltitude = 0;
	}
	
	free(originalPacketCopy);
		
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
	}
	return;
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

// Parses a string in the format: DDMM.MMMMMMM, where DD is degrees, and MM is minutes.
// degLength is the length of the degrees part. For example, degLength of 3 means
// the string will be DDDMM.MMMMMMM.
float parseDegreesMinutes(char *s, int degLength) {
	char degreesString[degLength + 1];
	float volatile degrees;
	float volatile minutes;
	// Copy the degrees part into degreesString and convert it to a float
	strncpy(degreesString, s, degLength);
	degreesString[degLength] = '\0';
	degrees = atof(degreesString);
	// Convert the minutes
	minutes = atof(s + degLength);
	// Convert the minutes to decimal degrees
	return degrees + (minutes / 60);
}

float getDegreesLong(char* longitude){
	if(*longitude == '\0') {
		return 0;
	}
	char degreesString[4];
	char minutesString[9];
	uint16_t volatile degrees;
	float volatile minutes;
	memcpy(degreesString,longitude,3);
	degreesString[3] = '\0';
	degrees = atoi(degreesString);
	memcpy(minutesString,longitude+3,8);
	minutesString[8] = '\0';
	minutes = atof(minutesString);
	minutes = minutes / 60;
	return (float)degrees + minutes;
}
	
float getDegreesLat(char* latitude){
	if(*latitude == '\0') {
		return 0;
	}
	char degreesString[3];
	char minutesString[9];
	uint16_t volatile degrees;
	float volatile minutes;
	memcpy(degreesString,latitude,2);
	degreesString[2] = '\0';
	degrees = atoi(degreesString);
	memcpy(minutesString,latitude+2,8);
	minutesString[8] = '\0';
	minutes = atof(minutesString);
	minutes = minutes / 60;
	return (float)degrees + minutes;
}