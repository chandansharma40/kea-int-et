/***********************/
// LCD DISPLAY DRIVER
/***********************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "defines.h"

int LCD_busy;
int useBusy;

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
// LCD Print TX
/***********************/
void LCD_prt (char x)
{
 	int dummy = 0; // Used for small delay
   
   /*count++; 
   
   if (count == 21)
   {
   LCD_goto(1, 2);
   }
   
   if (count == 41)
   {
   LCD_goto(1, 1);
   count = 1;
   }*/
    
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
// LCD Initialization
/***********************/

void LCD_init (void)
{
	_delay_ms(100);
	LCD_cmd(0x28);
	_delay_ms(10);	
	LCD_cmd(0x28);
	LCD_cmd(0x28);

	LCD_cmd(0x06);
	LCD_cmd(0x0C);
	LCD_cmd(0x01);	
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
	break;

	case 1: // Set BL pin
	bit_set(PORTD, BL);
	break;
	}
}




