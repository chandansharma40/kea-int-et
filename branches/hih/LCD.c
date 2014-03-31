/***********************/
// LCD Driver
/***********************/

#define F_CPU 16000000UL // 16 MHz clock speed

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

/***********************/
// Pin Definitions 
/***********************/	

	#define RS 0x04 	// LCD Register Select on PA2
	#define RW 0x08  	// LCD RW on PA3
	#define D0 0x10 	// LCD Data Pin 0 on PA4 
	#define D1 0x20 	// LCD Data Pin 1 on PA5
	#define D2 0x40 	// LCD Data Pin 2 on PA6
	#define D3 0x80  	// LCD Data Pin 3 on PA7
	#define E  0x40 	// LCD E on PC6
	#define BL 0x80   	// LCD Backlight on PD7 (via transistor switch)

/***********************/
// Variable Definitions
/***********************/

#define true 1
#define false 0
#define bool _Bool

int LCD_busy;
int useBusy;
char number;
char prev_number; // = 'D'; // 'D' is a dummy to enable check of previous character
int count;

char DEG[10];
char temp_string[10];
char light_string[10];
char time_string_ss[10];
char time_string_mm[10];
char time_string_hh[10];

//Interrupt variables
#define INTERVAL_TEMP 50;
#define INTERVAL_TIME 100;
#define INTERVAL_LIGHT 25;
#define INTERVAL_KEY 20;

char countTemp = 50;
char countTime = 10;
char countLight = 25;
char countKey = 20;

unsigned char ss=0, mm=0, hh=0;

bool runTemp = false;
bool runTime = false;
bool runLight = false;
bool runKey = false;


/***********************/
// Bit Manipulation
/***********************/

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))  // PORTA = PORTA or 0x08 (1 on pin 3) // bitset(PORTA, RS) => bitset(PORTA, 0x04) 
#define bit_clr(p,m) ((p) &= ~(m)) // PORTA = PORTA & 0x08 (!1 on pin3)
#define bit_flip(p,m) ((p) ^= (m)) // PORTA = PORTÂ ^ 0x08 (Toggle between 0 and 1 on pin 3)
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m)) 
#define BIT(x) (0x01 << (x)) 
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


/***********************/
// uP Initialization
/***********************/

void uP_init (void)
{
	DDRA = 0xFC; // Pin 2-7: LCD RS, RW, D0-D3
	DDRB = 0x08; // Pin 3: Sin curve
	DDRC = 0x40; // Pin 7: LCD E 
	DDRD = 0x80; // Pin 8: LCD Backlight
}


/***********************/
// Function Prototypes
/***********************/

void LCD_goto ();

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

void KEY_read()
 { unsigned int y,x,z;

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
 }

/***********************/
// Menu Program
/***********************/

void Menu (void)

//int Menu_prt;
	{
	LCD_string("1:Num 2:Temp 3:Light");
	//LCD_BL(1);
	
	}

/***********************/
// Sensors
/***********************/

int ADC_init (int channel)
{ switch (channel)
     { case 0:
       ADMUX &= ~(0b00000001);    // Initialize ADMUX, to select input and reference voltage
       break;
       case 1:
	   ADMUX |= 0b00000001;
	   break;
	  }

  
     ADCSRA = 0b10010111;    // Initialize the control and status register
     ADCSRA |=0b01000000;    // Start the conversion   
        
     while ((ADCSRA & 0x10) == 0);

    return (ADC);
}

/***********************/
//TEMP_value
/***********************/

void TEMP_value (void)

{
	int adc_result;
	int DEG_value = 84;
	int dummy;
	int DEG;
	int ADC_steps = 4;

     ADC_init(0);		//temp = 0
	  //_delay_ms(100);
      adc_result = ADC;          
     // itoa(adc_result, temp_string, 10);    //convert integer into a string
	  	dummy = adc_result - DEG_value;
	  	DEG = dummy / ADC_steps;
		itoa(DEG, temp_string, 10);
}

/***********************/
//LIGHT_value
/***********************/
void LIGHT_value (void)

{
	int adc_result;

	ADC_init(1);		//light = 1
	  adc_result = ADC;
	  itoa(adc_result, light_string, 10);

}
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
// Main Program
/***********************/

int main (void)
 {  
	int toggle = 1;
   uP_init(); // Microprocessor init
	LCD_init(); // LCD init
	LCD_BL(toggle); // Turn on backlight
	LCD_cmd(0x0F); // Blink cursor
	timer1_init();
	//Menu();
		
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
		TEMP_value(); // call the task function
		LCD_string(temp_string);
		runTemp = false; // clear the run flag
		//LCD_clr();
	}
	//Light
	if (runLight)
	{
		LCD_goto(5,2);
		LIGHT_value(); // call the task function
		LCD_string(light_string);
		runLight = false; // clear the run flag
	}
	//Time
	if (runTime)
	{
		
		TIME_value(); // call the task function
		
		LCD_goto(7,1);
		itoa(ss, time_string_ss, 10);
		LCD_string(time_string_ss);

		LCD_goto(4,1);
		itoa(mm, time_string_mm, 10);
		LCD_string(time_string_mm);

		LCD_goto(1,1);
		itoa(hh, time_string_hh, 10);
		LCD_string(time_string_hh);

		runTime = false; // clear the run flag
	}



/*
      // Check key pressed
    KEY_read();

      // if key is not the same as previous key, write to display
     if (number != prev_number)
      {
     switch(number) // Switch to check type of input
      { 
         case 0:
           break;
         default:
		 		
           break;

		   case '1':
		   
         
         if(number != '#'){
			
			KEY_read();
			rows_init();
			col_init();
			switch(number);
			{
			
         LCD_prt(number);
			}		   	

			case '*':
				{
           		LCD_clr(); // Clear display
           		count = 0; // Reset counter for cursor position
				}
		    }
		   break;

		   case '2':
		   if(number != '#'){
					// int start_time = time
					//	if (start_time + 1000ms == time)
		   	{
			LCD_clr();
		   LCD_string("Temp:");
			TEMP_value();
			LCD_string(temp_string);
			}
					//	else
			{
			KEY_read();
			}
			_delay_ms(500);
		    
		    }
		    
		    break;

		   case '3':
		   if(number != '#'){
		   {
		   LCD_clr();
		   LCD_string("Light:");
		   LIGHT_value();
		   LCD_string(light_string);
		   }

		   KEY_read();
		   _delay_ms(500);
		   }
		   
		   break;
		    

        case '#':
		   {
		   ISR(TIMER1_COMPA_vect);
         LCD_clr();
		   Menu();
		   break;
			}


		 case '0':
		   
         {
		   toggle = !toggle; // Toggle on/off
		   LCD_BL(toggle) ; // Change backlight status
         _delay_ms(100);
         break; 
         }
	   	
       prev_number = number;
      }
      }
      // if key is the same, do nothing
      else
      {
      }        
      _delay_ms(20); // Delay between key reads  
	  */
  }

  
}

