/***********************/
// LCD DISPLAY DRIVER
/***********************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "defines.h"
#include "LCD.h"

int LCD_busy;
int useBusy;
bool lcd_backlight = true;




/***********************/
// Custom Characters
/***********************/
void LCD_CreateCustomCharacters (void) 
{

	// make CGRAM data available from MPU and set custom characters in CGRAM 1-5
// make CGRAM data available from MPU and set custom characters in CGRAM 1-5

LCD_cmd (0x40); //starts customization at first CGRAM place

// 0 bar character

LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);
LCD_prt (0x00);

// 1 bar character

LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x10);
LCD_prt (0x00);

// 2 bar character

LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x18);
LCD_prt (0x00);

// 3 bar character

LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x1C);
LCD_prt (0x00);

// 4 bar character

LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x1E);
LCD_prt (0x00);

//5 bar character

LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x1F);
LCD_prt (0x00);

// Degree sign
LCD_prt (0x02); // 8
LCD_prt (0x05); // 7
LCD_prt (0x02); // 6
LCD_prt (0x00); // 5
LCD_prt (0x00); // 4
LCD_prt (0x00); // 3
LCD_prt (0x00); // 2
LCD_prt (0x00); // 1

LCD_cmd (0x80); //returns to DDRAM
}


/***********************/
// LCD Command TX
/***********************/
void LCD_cmd (char x)
{
 	int dummy = 0; // Used for small delay

	// Split 8-bit command into 2 bytes with 4 bits each
	char MSB = x & 0xF0; // mask out LSB;
	char LSB = x << 4; // move LSB up to MSB

	// Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
	bit_clr(PORTA, RS);
	
	// Set RW = 0 to write to LCD, RW = 1 to read from LCD
	bit_clr(PORTA, RW);

	// Set E = 1
	bit_set(PORTC, 0x40);

	// Send MSB on DB4-7
	// PORTA = (PORTA & 0x0F) | MSB
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= MSB;
	
	// Set E = 0
	bit_clr(PORTC, E);

	dummy++; // Small delay

	// Set E = 1
	bit_set(PORTC, E);
	
	// Send LSB on DB4-7
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= LSB;
	
	// Set E = 0
	bit_clr(PORTC, E);
	
	_delay_ms(2);
}

/***********************/
// LCD Clear screen
/***********************/
void LCD_Clear(void)
{
	// Clear all text on display, set cursor to (1,1)
	LCD_cmd(0x1);
}

/***********************/
// LCD Progess bar
/***********************/
void LCD_DrawBar(unsigned char percent)
{
	float test = percent * 0.4;
	char *progessBar = malloc(sizeof(char)*9);
	unsigned char counter = 0;

	// Clear out bar first with spaces
	memset(progessBar, 0x20, 8);

	if ( (percent < 100) && (percent > 0))
	{
		while (test >= 5)
		{
			progessBar[counter] = (unsigned char)0x5;
			test -= 5;
			counter++;
		}

		if (test > 0)
		{
			progessBar[counter] =test;
			counter++;
		}

		progessBar[8] = '\0';
		LCD_string(progessBar);
		free(progessBar);
	}

}

/***********************/
// LCD Print TX
/***********************/
void LCD_prt (char x)
{
 	int dummy = 0; // Used for small delay
   
	// Split 8-bit command into 2 bytes with 4 bits each
	char MSB = x & 0xF0; // mask out LSB;
	char LSB = x << 4; // move LSB up to MSB

	// Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
	bit_set(PORTA, RS);
	
	// Set RW = 0 to write to LCD, RW = 1 to read from LCD
	bit_clr(PORTA, RW);

	// Set E = 1
	bit_set(PORTC, 0x40);

	// Send MSB on DB4-7
	// PORTA = (PORTA & 0x0F) | MSB
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= MSB;
	
	// Set E = 0
	bit_clr(PORTC, E);

	dummy++; // Small delay

	// Set E = 1
	bit_set(PORTC, E);
	
	// Send LSB on DB4-7
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= LSB;
	
	// Set E = 0
	bit_clr(PORTC, E);
	
	_delay_ms(2);
}

/***********************/
// LCD internal reset
/***********************/
void LCD_reset(void)
{
	_delay_ms(100);
	LCD_cmd(0x30);
	_delay_ms(20);
	LCD_cmd(0x30);
	_delay_ms(10);
	LCD_cmd(0x30);

	_delay_ms(50);
}

/***********************/
// LCD Initialization
/***********************/

void LCD_init (void)
{
	// RESET LCD before going into initialization plzz
	LCD_reset();

	// Function set - 4bit interface
	LCD_cmd(0x28);
	_delay_ms(10);

	// Display off
	LCD_cmd(0x08);
	_delay_ms(10);

	// Display clear
	LCD_cmd(0x01);
	_delay_ms(10);

	// Entry mode set, increment, shifting off
	LCD_cmd(0x06);
	_delay_ms(10);

	// Done with initialization, turn on display again
	// Display off
	LCD_cmd(0x0C);
	_delay_ms(10);
		
    LCD_CreateCustomCharacters();
}

/***********************/
// LCD Print String A
/***********************/

void LCD_string(char *str)
{
   while(*str > 0)
 {	
   LCD_prt(*str++);
   }
}


/***********************/
// LCD Move left or right
/***********************/
void LCD_move(int x)
{
	switch(x)	
	{
	case 0:
	LCD_cmd(0x10); // Move cursor to the left
	break;
	
	case 1:
	LCD_cmd(0x14); // Move cursor to the right
	break;
	}
}


/***********************/
// LCD Goto XY Position
/***********************/
void LCD_goto(int x, int y)
{
	switch(y)
	{
	case 1: // Goto x value on first line
	LCD_cmd(0x80| (x-1));
	break;

	case 2: // Goto x value on second line
	x += 40;
	LCD_cmd(0x80| (x-1));
	break;
	}
}

/***********************/
// LCD Display Clear
/***********************/

void LCD_clr()
{
	LCD_cmd(0x01);
}

/***********************/
// LCD Backlight On/Off
/***********************/

void LCD_BL(int x)
{
	switch(x)
	{
	case 0: // Clear BL pin
	bit_clr(PORTD, BL);
	lcd_backlight = false;
	break;

	case 1: // Set BL pin
	bit_set(PORTD, BL);
	lcd_backlight = true;
	break;
	}
}

/***********************/
// LCD Backlight On/Off
/***********************/

bool LCD_IsBacklightOn(void)
{
	return lcd_backlight;
}




