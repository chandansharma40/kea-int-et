/***********************/
// MAIN Program
/***********************/
#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>	
#include "kbd.h"
#include "LCD.h"
#include "sensors.h"
#include "clock.h"


//Interrupt variables
#define INTERVAL_TEMP 50
#define INTERVAL_TIME 100
#define INTERVAL_LIGHT 25
#define INTERVAL_KEY 20


// Task/Schedule variabls
bool runTemp = false;
bool runTime = false;
bool runLight = false;
bool runKey = false;

// Timing variables
char countTemp = 50;
char countTime = 10;
char countLight = 25;
char countKey = 20;

// Keyboard variabls
char keyPressed;
char prevKeyPressed; // = 'D'; // 'D' is a dummy to enable check of previous character

/***********************/
// Interrupt
/***********************/
ISR(TIMER1_COMPA_vect)
{
	//Temperature
	if ((--countTemp) == 0)
	{
		countTemp = INTERVAL_TEMP; // reload the counter
		runTemp = true; // set the ready-flag for the task
	}
	//Time
	if ((--countTime) == 0)
	{
		countTime = INTERVAL_TIME; // reload the counter
		runTime = true; // set the ready-flag for the task
	}
	//Light
	if ((--countLight) == 0)
	{
		countLight = INTERVAL_LIGHT; // reload the counter
		runLight = true; // set the ready-flag for the task
	}
	//Keyboard
	if ((--countKey) == 0)
	{
		countKey = INTERVAL_KEY; // reload the counter
		runKey = true; // set the ready-flag for the task
	}
}

/***********************/
// µP Initialization 
/***********************/
void uP_init(void)
{
	DDRA = 0xFC; // Pin 2-7: LCD RS, RW, D0-D3
	DDRB = 0x09; // Pin 3: Sin Curve
	DDRC = 0x40; // Pin 7: LCD E
	DDRD = 0x80; // Pin 8: LCD Backlight
}


/***********************/
// Main function (Program starts here) 
/***********************/
int main (void)
{  

	int toggle = 1;
  	uP_init(); // Microprocessor init
	LCD_init(); // LCD init
	LCD_BL(toggle); // Turn on backlight
	LCD_cmd(0x0F); // Blink cursor
	timer1_init();

	
	// Set up our menu
	LCD_goto(3,1);
	LCD_string(":");
	LCD_goto(6,1);
	LCD_string(":");
	LCD_goto(11,1);
	LCD_string("T:");
	LCD_goto(15,1);
	LCD_string(".00");
	LCD_goto(18,1);
	LCD_string("*");
	LCD_goto(1,2);
	LCD_string("LDR:");

	while (1)
	{

	//Temperature
	if (runTemp)
	{
		LCD_goto(13,1);

		// Read temp, convert it and print it
		int temp = TEMP_value(); // call the task function
		char temp_str[4];
		itoa(temp, temp_str, 4);
		LCD_string(temp_str);

		runTemp = false; // clear the run flag
	}

	//Light
	if (runLight)
	{
		LCD_goto(5,2);

		// Read Light, convert it and print it
		int light_value = LIGHT_value(); // call the task function
		char light_str[4];
		itoa(light_value, light_str, 4);
		LCD_string(light_str);

		runLight = false; // clear the run flag
	}

	//Time
	if (runTime)
	{
		TIME_Tick(); // call the task function
		char time_string[4];

		// Seconds
		LCD_goto(7,1);
		itoa(TIME_GetSeconds(), time_string, 4);
		LCD_string(time_string);

		// Minutes
		LCD_goto(4,1);
		itoa(TIME_GetMinutes(), time_string, 4);
		LCD_string(time_string);

 		// Hours
		LCD_goto(1,1);
		itoa(TIME_GetHours(), time_string, 4);
		LCD_string(time_string);

		runTime = false; // clear the run flag

	}

	// Keyboard handler
	if (runKey)
	{
		// If any key is pressed
		if (KBD_isKeyPressed())
		{
			keyPressed = KBD_GetKey();
		}
	}
    }
}
