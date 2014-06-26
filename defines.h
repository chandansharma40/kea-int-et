#ifndef DEFINES_H_
#define	DEFINES_H_

/***********************/
// GLOBAL defines
/***********************/



/***********************/
// Variable Definitions
/***********************/

#define true 1
#define false 0
#define bool _Bool

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
// Bit Manipulation
/***********************/

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))  // PORTA = PORTA or 0x08 (1 on pin 3) // bitset(PORTA, RS) => bitset(PORTA, 0x04) 
#define bit_clr(p,m) ((p) &= ~(m)) // PORTA = PORTA & 0x08 (!1 on pin3)
#define bit_flip(p,m) ((p) ^= (m)) // PORTA = PORTÂ ^ 0x08 (Toggle between 0 and 1 on pin 3)
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m)) 
#define BIT(x) (0x01 << (x)) 
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

#endif
