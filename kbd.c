/***********************/
// Keyboard Driver
/***********************/

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include "kbd.h"
#include "defines.h"

char oldResult;

char KBDReadX(void)
{
	char x = 0;
	char dummy = 0;

	_delay_us(10);

	// scan for x
	// Set port b for input with pull up
	DDRB = DDRB & ~(0x7);
	PORTB = PORTB | 0x7;


	// Set port d for output, and low output
	DDRD = DDRD | 0x78;
	PORTD = PORTD & ~(0x78);

	_delay_us(10);

	x = PINB & 0x7;
	x = 0x7 ^ x;

	if (x==1) return 0;
	else if(x==2) return 1;
	else if (x==4) return 2;

	return 0;
}

char KBDReadY(void)
{
	char y = 0;
	char dummy = 0;

	// Scan for y
	// Set port d for input with pull-up
	DDRD = DDRD & ~(0x78);
	PORTD = PORTD | 0x78;

	// Set port b for output, low output
	DDRB = DDRB | 0x7;
	PORTB = PORTB & ~(0x7);

	_delay_us(10);

	y = PIND & 0x78;
	y = (0x78 ^ y)>>3;
	



	if (y==1) return 0;
	else if(y==2) return 1;
	else if (y==4) return 2;
	else if (y==8) return 3;


	return 0;
}


bool KBD_isKeyPressed(void)
{
	// Read X if any changes, call KeyBoardScan()
	char tmpx = 0;
	char dummy = 0;

	_delay_us(10);

	DDRB = 0x0;
	PORTB = 0x7;

	_delay_us(10);

	DDRD = DDRD | 0x78;
	PORTD = PORTD & ~0x78;

	_delay_us(10);

	tmpx = PINB & 0x7;

	if (tmpx != 0x07)
		return true;
	else
	{
		return false;
	}
}

char KBD_GetKey(void)
{
	const char KeyArray[] = {'1','2','3','4','5','6','7','8','9','*','0','#'};
	char res = 0;
	char dummy = 0;
	int i = 0;
	for (; i < 10; i++)
	{
		// x returns number 0..2 
		// y returns number 0..3
		char x = KBDReadX();
		char y = KBDReadY();
		
		// See notes for KBD
		// 3x4 kbd
		res = 3*y + x;

		if (i==0)
			oldResult = res;
		
		_delay_ms(10);
	}


	// If result still is the same AND is the result smaller or equal to the array size
	// If result is above it will result in a buffer overflow, and that is not wanted!
	if ((res == oldResult) && (res <= 11))
		return KeyArray[res];
	else
		return 0;
}


