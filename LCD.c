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
bool lcd_backlight = true;


/***********************/
// Custom Characters
/***********************/
void LCD_BuildCharacter (char adress, char character[]) 
{
	if (adress < 8)
	{
		LCD_cmd (0x40 + adress*8);	// Set adress to CGRAM start + offset
	}

	for (int i =0; i < 7; i++)  // Print 8x1 byte
	{
		LCD_prt(character[i]);
	}

	LCD_cmd (0x80); //returns to DDRAM
}

/***********************/
// Custom Characters
/***********************/
void LCD_CreateCustomCharacters (void) 
{

	// make CGRAM data available from MPU and set custom characters in CGRAM 1-5

	// 0 bar character
	// Is this neeeded?? <--- Jan
	char char_buff_bar0[8] = { 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00};
	LCD_BuildCharacter(0, char_buff_bar0);

	// 1 bar character
	char char_buff_bar1[8] = { 0x10, 0x10, 0x10, 0x10, 0x10,0x10, 0x10, 0x00};
	LCD_BuildCharacter(1, char_buff_bar1);

	// 2 bar character
	char char_buff_bar2[8] = { 0x18, 0x18, 0x18, 0x18, 0x18,0x18, 0x18, 0x00};
	LCD_BuildCharacter(2, char_buff_bar2);

	// 3 bar character
	char char_buff_bar3[8] = { 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,0x1C, 0x1C, 0x00};
	LCD_BuildCharacter(3, char_buff_bar3);

	// 4 bar character
	char char_buff_bar4[8] = { 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,0x1E, 0x1E, 0x00};
	LCD_BuildCharacter(4, char_buff_bar4);

	//5 bar character
	char char_buff_bar5[8] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,0x1F, 0x1F, 0x00};
	LCD_BuildCharacter(5, char_buff_bar5);

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
// LCD Progess bar
/***********************/
void LCD_DrawBar(unsigned char percent)
{
	float test = percent * 0.4;
	char progessBar[9];
	char counter = 0;

	if ( (percent < 100) && (percent > 0))
	{
		while (test >= 5)
		{
			progessBar[counter] = '5';
			test -= 5;
			counter++;
		}

		if (test > 0)
		{
			progessBar[counter] = '0' + (char)test;
			counter++;
		}

		for (counter; counter != 8; counter++)
			progessBar[counter] = ' ';

		progessBar[counter+1] = '\0';
		LCD_string(progessBar);
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




