/*
 * HARTCode.c
 *
 * Created: 4/30/2017 6:22:43 PM
 * Author : Scott Merrill
 */

#include "unittest.h"
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	#ifdef DOUNITTEST
	beginUnitTest();
	#endif

	while(1){};
}
