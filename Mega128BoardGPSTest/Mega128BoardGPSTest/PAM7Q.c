#include "PAM7Q.h"
#include "unittest.h"

char volatile gpsBuffer[256];
uint8_t volatile msgIndex = 0;
uint8_t volatile msgBeginFlag = 0;
uint8_t volatile msgEndFlag = 0;
#ifdef UNITTEST
uint8_t volatile commaCount = 0;
#endif

ISR(USART0_RX_vect){
	uint8_t rcvb;
	rcvb = UDR0;
	#ifdef DOUNITTEST
	PORTB &= ~(1 << 3);
	PORTB ^= (1 << 0);
	//USARTTX(rcvb, GPSPORT);
	#endif
	//Resets if too high
	if (msgIndex == 255){
		#ifdef DOUNITTEST
		PORTB &= ~(1 << 2);
		#endif
		msgIndex = 0;
		msgBeginFlag = 0;
		msgEndFlag = 0;
	}
	//Checks to see receive byte is start of packet
	if (rcvb == '$' && !msgEndFlag){
		//If it is sets begin flag, puts in buffer
		#ifdef DOUNITTEST 
		PORTB |= (1 << 1);
		PORTB &= ~(1 << 3);
		PORTB &= ~(1 << 2);
		commaCount = 0;
		#endif
		msgBeginFlag = 1;
		msgIndex = 0;
		gpsBuffer[msgIndex] = rcvb;
		msgIndex++;
	} else if (msgBeginFlag && rcvb != '*' && !msgEndFlag && rcvb != '$'){
		//If the message has started, put all received stuff in buffer
		#ifdef DOUNITTEST 
		PORTB |= (1 << 2);
		PORTB &= ~(1 << 1);
		if (rcvb == ','){
			commaCount++;
		}
		#endif
		gpsBuffer[msgIndex] = rcvb;
		msgIndex++;
	} else if (msgBeginFlag && rcvb == '*' && !msgEndFlag){
		//If end, stop receiving stuff and set end flag so that parsing can occur
		#ifdef DOUNITTEST 
		PORTB |= (1 << 3);
		PORTB &= ~(1 << 1);
		PORTB &= ~(1 << 2);
		#endif
		gpsBuffer[msgIndex] = rcvb;
		msgIndex++;
		gpsBuffer[msgIndex] = '\0';
		msgIndex = 0;
		msgEndFlag = 1;
		msgBeginFlag = 0;
		cli();
		return;
	}
}

//Use RATE (PUBX,40)
uint16_t InitGPS(void){
	uint16_t volatile SetUBRR; //Turns off all the messages we don't want
	msgIndex = 0;
	msgBeginFlag = 0;
	msgEndFlag = 0;
	char CFGMSG[CFGMSGSIZE] = PUBXNOCOMMCFGMSGBASE;
	SetUBRR = InitUSART(GPSBAUD, GPSPORT);
	if (SetUBRR){
		_delay_ms(2000);
		PUBXCFGSetup(CFGMSG, "GLL");
		SendGPS(CFGMSG, CFGMSGSIZE);
		_delay_ms(300);
		PUBXCFGSetup(CFGMSG, "GSA");
		SendGPS(CFGMSG, CFGMSGSIZE);
		_delay_ms(300);
		PUBXCFGSetup(CFGMSG, "GSV");
		SendGPS(CFGMSG, CFGMSGSIZE);
		_delay_ms(300);
		PUBXCFGSetup(CFGMSG, "RMC");
		SendGPS(CFGMSG, CFGMSGSIZE);
		_delay_ms(300);
		PUBXCFGSetup(CFGMSG, "VTG");
		SendGPS(CFGMSG, CFGMSGSIZE);
		sei();
		return SetUBRR;
	} else {
		return 0;
	}
}

// Sets up the messages that disable various NMEA messages
// Parameters: 
//	packet: The packet to be sent, consisting of the config message base. The message name gets modified.
//	msg: The particular identifier of the message to be turned off
// Returns:
//	Nothing

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

// Calculates and writes the checksum for an outgoing packet
void CheckSum(char* packet){
	uint8_t volatile i = 0;
	uint8_t volatile checksum;
	char hexchar[3];
	while(!(packet[i] == '*')){
		checksum ^= packet[i]; //XORs all the packet bytes together to get the checksum
		i++;
	}
	sprintf(hexchar, "%02X", checksum);
	i++;
	packet[i] = hexchar[0];
	i++;
	packet[i] = hexchar[1];
	return;
}

// Once message end flag is set, puts data in the GPS struct and resets end flag
// Parameters:
//		GPSdata:	Struct that accepts data
//	Returns:
//		Nothing
void getGPSData(struct GPSStruct *GPSdata){
	#ifdef DOUNITTEST
	uint8_t volatile echoLength = 0;
	uint8_t volatile echoIndex = 0;
	int32_t volatile latConvert = 0; 
	int32_t volatile longConvert = 0;
	char echoLatLongAlt[256];
	#endif
	if (msgEndFlag){
		parseGGA(gpsBuffer, GPSdata);
		msgEndFlag = 0;
		#ifdef DOUNITTEST
		PORTB &= ~(1 << 3);
		latConvert = (int32_t)GPSdata->latitude;
		longConvert = (int32_t)GPSdata->longitude;
		echoLength = sprintf(echoLatLongAlt, " %ld.%ld %ld.%ld %u %u", latConvert, labs((int32_t)((GPSdata->latitude - latConvert)*100000)), longConvert, labs((int32_t)((GPSdata->longitude - longConvert)*100000)), GPSdata->GPSAltitude, commaCount);
		for (echoIndex; echoIndex < echoLength; echoIndex++){
			USARTTX(echoLatLongAlt[echoIndex], GPSPORT);
		}
		USARTTX('\n', GPSPORT);
		#endif
		sei();
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
	if(*msgPart == 'W') {
		GPSdata->longitude = -GPSdata->longitude;
	}
	
	// Skip the quality, numSV, and HDOP fields
	for(i = 0; i < 3; i++) {
		strsep(&packetCopy, ",");
	}
	
	// Get the altitude. If there is no altitude, then set it to zero.
	msgPart = strsep(&packetCopy, ",");
	if(*msgPart != '\0') {
		GPSdata->GPSAltitude = atof(msgPart);
	} else {
		GPSdata->GPSAltitude = 0;
	}
	
	free(originalPacketCopy);
		
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
