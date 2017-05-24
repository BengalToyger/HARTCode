#include "unittest.h"

#ifdef DOUNITTEST

#include <time.h>

unsigned long randContext;

// This function should be called when a unit test fails. It only exists so we can put a
// breakpoint inside it and observe the failure. There are multiple
// failUnitTest functions that cascade different numbers of parameters can
// be sent.
void failUnitTest2(char *error1, char *error2) {
	volatile int foo = 0;
}

void failUnitTest1(char *error) {
	failUnitTest2(error, NULL);
}

void makeRandomDegreesMinutesCall(int maxDegrees, int length) {
	char s[12];
	int deg, min, minFraction1, minFraction2;
	float result,  expected;

	deg = rand_r(&randContext) % maxDegrees;
	min = rand_r(&randContext) % 60;
	//minFraction = (rand_r(&randContext) % 99999) / 10000.0;
	minFraction1 = rand_r(&randContext) % 100;
	minFraction2 = rand_r(&randContext) % 1000;
	expected = deg + (min + minFraction1 / 100.0 + minFraction2 / 100000.0) / 60.0;

	if(length == 2) {
		sprintf(s, "%02d%02d.%02d%03d", deg, min, minFraction1, minFraction2);
	} else if (length == 3) {
		sprintf(s, "%03d%02d.%02d%03d", deg, min, minFraction1, minFraction2);
	}
	result = parseDegreesMinutes(s, length);

	if (abs(result - expected) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest2("Failed in makeRandomDegreesMinutesCall", s);
	}
}

void testParseDegreesMinutes() {

	// Test 3 digit degrees, all zeroes
	if(abs(parseDegreesMinutes("00000.00000", 3)) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 00000.00000");
	}

	// Test 2 digit degrees, all zeroes
	if(abs(parseDegreesMinutes("0000.00000", 2)) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 0000.00000");
	}

	// Happy path for 3 digit degrees
	if(abs(parseDegreesMinutes("12045.96395", 3) - 120.76606) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 12045.96395");
	}

	// Happy path for 2 digit degrees
	if(abs(parseDegreesMinutes("2212.35482", 2) - 22.20591) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 12045.96395");
	}

	// Tests that leading zeroes in degrees works OK
	if(abs(parseDegreesMinutes("00123.07353", 3) - 1.38455) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 12045.96395");
	}

	// Tests that leading zeroes in minutes works OK
	if(abs(parseDegreesMinutes("08602.93524", 3) - 86.04892) > PARSE_DEGREES_MINUTES_TOLERANCE) {
		failUnitTest1("testParseDegreesMinutes failed with 12045.96395");
	}

	// Test maximum values

	// random tests
	for(int i = 0; i < 10000; i++) {
		makeRandomDegreesMinutesCall(90, 2);
		makeRandomDegreesMinutesCall(180, 3);
	}

}

void beginUnitTest() {
	srand(time(NULL));
	testParseDegreesMinutes();
}

#endif
