#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>


/***********************/
// Initialize keyboard 
/***********************/

void rows_init()
{
   DDRD &= 0b10000111;   //set rows as input
   PORTD |= 0b01111000;  // pull up resistors
   DDRB |= 0b00000111;   // set columns as output
   PORTB &= 0b11111000;  // set colummns low
   _delay_us(10);
}

void col_init()
{
   DDRB &= 0b11111000;   //set columns as input
   PORTB |= 0b00000111;  // pull up resistors
   DDRD |= 0b01111000;   // set rows as output
   PORTD &= 0b10000111;  // set rows low
   _delay_us(10);
}

int KEY_read()
 {
 unsigned int y,x,z;
unsigned int number;

   rows_init();
   y = PIND | 0b10000111; //read rows inputs
   _delay_ms(10);         // debounce delay

   col_init();
   x = PINB | 0b11111000; //read col inputs
   _delay_ms(10);         // debounce delay
   
   z=x&y; 
   switch (z)
     {   
      case 0b10111110:  //if row 4 and col 3 is pressed
       number = '*';
       break;
      case 0b10111011:
       number = '#';
       break;
      case 0b10111101:
       number = 0x30;
       break;
      case 0b11011110:
       number = 0x37;
       break;
      case 0b11011101:
       number = 0x38;
       break;
      case 0b11011011:
       number = 0x39;
       break;
      case 0b11101110:
       number = 0x34;
       break;
      case 0b11101101:
       number = 0x35;
       break;
      case 0b11101011:
       number = 0x36;
       break;
      case 0b11110110:
       number = 0x31;
       break;
      case 0b11110101:
       number = 0x32;
       break;
      case 0b11110011:
       number = 0x33;
       break;
       default:
       number = 0;
       }
      _delay_ms(10);

	return number;
 }

