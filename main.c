/***********************/
// MAIN Program
/***********************/
#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>	
#include <stdio.h>
#include "kbd.h"
#include "LCD.h"
#include "sensors.h"
#include "clock.h"
#include "keyboard.h"


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
unsigned char keyPressed;
unsigned char prevKeyPressed = 'D'; // 'D' is a dummy to enable check of previous character

// TESTING VARIABLES <------- DELETE THIS @ SOME TIME
char test_counter = 0;

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

	LCD_goto(9,2);
	LCD_string("ON");

	
	while (1)
	{

		//Temperature
		if (runTemp)
		{
			LCD_goto(11,1);

			// Read temp, convert it and print it
			int temp = TEMP_value(); // call the task function
			char temp_str[4];
			itoa(temp, temp_str, 10);
			LCD_string("T:");
			LCD_string(temp_str);

			runTemp = false; // clear the run flag
		}

		//Light
		if (runLight)
		{
			LCD_goto(1,2);
			LCD_string("LDR:");

			// Read Light, convert it and print it
			int light_value = LIGHT_value(); // call the task function
			char light_str[4];
			sprintf(light_str, "%03d", light_value);
			LCD_string(light_str);


	  		LCD_goto(13,2);

			// 500 represents light max!!
			unsigned int light_percent = (light_value * 100) / 500;
		/*	if (light_percent < 0)
				light_percent = 1;
			else if(light_percent > 100)
				light_percent = 99;
*/
			LCD_DrawBar(light_percent);



			runLight = false; // clear the run flag
		}


		//Time
		if (runTime)
		{
			TIME_Tick(); // call the task function
			char *test;

			LCD_goto(1,1);
			sprintf(test, "%02d:%02d:%02d", TIME_GetHours(), TIME_GetMinutes(), TIME_GetSeconds()); 
			LCD_string(test);


			runTime = false; // clear the run flag

		}

		// Keyboard handler
		if (runKey)
		{
				keyPressed = KBD_GetKey();
			    if (keyPressed != 0)
					{
					switch(keyPressed)
					{
						case '#': // set time
								if (prevKeyPressed == '#')
								{
										cli();
										bool setTime = true;
										bool gettingInput = true;
										unsigned char digits[6];

										LCD_goto(1, 1);
										LCD_string("--:--:--");
										LCD_goto(1, 1);
									
										unsigned char cDigit = 0;
										unsigned char digit_count = 0;

									
									
										while (gettingInput)
										{						
										
											if (KBD_isKeyPressed())
												keyPressed = KBD_GetKey() ;

											if ( (keyPressed >= 48) && (keyPressed <= 56))
											{
												// Get digit
												digits[digit_count] = keyPressed;
												digit_count++;
												LCD_prt(keyPressed);
												
												// If we have all digits end
												if (digit_count == 6)
												{
													gettingInput = false;
												}

												switch(digit_count)
												{
													case 1:
														LCD_goto(2,1);
														break;
													case 2:
														LCD_goto(4,1);
														break;
													case 3:
														LCD_goto(5,1);
														break;
													case 4:
														LCD_goto(7,1);
														break;
													case 5:
														LCD_goto(8,1);
														break;
												}
							
											// Wait 0.4 sec
											keyPressed = 0;
											_delay_ms(300);
										}
											// Repeat
									}

									// Now we got em all, lets attach them to the clock	
									unsigned char buffer[3];
									buffer[0] = digits[0];
									buffer[1] = digits[1];
									buffer[2] = '\0';

									TIME_SetHours(atoi(buffer));
									
									buffer[0] = digits[2];
									buffer[1] = digits[3];
									TIME_SetMinutes(atoi(buffer));
									
									buffer[0] = digits[4];
									buffer[1] = digits[5];
									TIME_SetSeconds(atoi(buffer));
																										
									setTime = false;
									// Enable interrupts again
									sei();
																
								}
						break;
						case '*': // Toggle LCD backlight on/off
								LCD_goto(9,2);
								if (LCD_IsBacklightOn() == true)
								{
									LCD_BL(0);
									LCD_string("OFF");
								}
								else
								{
									LCD_BL(1);
									LCD_string("ON ");
								}
						break;
					}

				prevKeyPressed = keyPressed;
			}
	    }
	}
}
