#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"

unsigned char ss=0, mm=0, hh=0;

/***********************/
//TIME_value
/***********************/
void TIME_value (void)
{
	// Every 10 ms, 
	ss++;
	if (ss >= 60)
	{
		ss=0;
		mm++;
	}
	if (mm >= 60)
	{
		hh++;
		mm=0;
	}
	if (hh == 24)
	{
		hh = 0;
	}
}

/***********************/
//TIME_value
/***********************/
int TIME_GetSeconds (void)
{
	return ss;
}
/***********************/
//TIME_value
/***********************/
int TIME_GetMinutes (void)
{
	return mm;
}
/***********************/
//TIME_value
/***********************/
int TIME_GetHours (void)
{
	return hh;
}

/***********************/
//Timer / clock
/***********************/
void timer1_init()
{
	TCCR1A = 0b00000000;

	TCCR1B = 0b00001010;

    TIMSK  = 0b00010000;

	OCR1A = 19999; //gives interrupt every 10ms
	
	TCNT1  = 0b00000000;
	
	sei(); // enable global interrupts
}
