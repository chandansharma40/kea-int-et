/***********************/
// Clock Driver
/***********************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"

/***********************/
// Custom data type, used to handle time
/***********************/
struct Clock_time
{
	int seconds;
	int minutes;
	int hours ;
} ;

struct Clock_time clock_time;

/***********************/
//TIME_Tick, Keeps control of time, ! Interrupt should call this function every second
/***********************/
void TIME_Tick (void)
{
	// Every 1000 ms, 
	clock_time.seconds++;

	if (clock_time.seconds >= 60)
	{
 		clock_time.seconds=0;
		clock_time.minutes++;
	}
	if (clock_time.minutes >= 60)
	{
		clock_time.hours++;
		clock_time.minutes=0;
	}
	if (clock_time.hours >= 24)
	{
		clock_time.hours = 0;
		clock_time.minutes = 0;
		clock_time.seconds = 0;
	}
}

/***********************/
//TIME_GetSeconds -> Returns seconds
/***********************/
int TIME_GetSeconds (void)
{
	return clock_time.seconds;
}
/***********************/
//TIME_GetMinutes -> Returns minutes
/***********************/
int TIME_GetMinutes (void)
{
	return clock_time.minutes;;
}
/***********************/
//TIME_GetHours -> Returns hours
/***********************/
int TIME_GetHours (void)
{
	return clock_time.hours;
}

/***********************/
//TIME_SetSeconds -> Set Seconds to value given
/***********************/
void TIME_SetSeconds (int value)
{
	if (value < 60)
		clock_time.seconds = value;
}

/***********************/
//TIME_SetMinutes -> Set Minutes to value given
/***********************/
void TIME_SetMinutes (int value)
{
	if (value < 60)
		clock_time.minutes = value;
}

/***********************/
//TIME_SetHours -> Set Hours to value given
/***********************/
void TIME_SetHours (int value)
{
	if (value < 24)
		clock_time.hours = value;
}


/***********************/
// Initialization of Timer / clock
/***********************/
void timer1_init()
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b00001010;
	TIMSK  = 0b00010000;

	OCR1A = 19999; //gives interrupt every 10ms
	
	TCNT1  = 0b00000000;


	sei(); // enable global interrupts

	clock_time.seconds = 0;
	clock_time.minutes = 0;
	clock_time.hours   = 0;

}
