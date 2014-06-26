#ifndef LCD_H_
#define LCD_H_

// POSITION IN CGRAM
#define BAR_CHAR0 0x0
#define BAR_CHAR1 0x1
#define BAR_CHAR2 0x2
#define BAR_CHAR3 0x3
#define BAR_CHAR4 0x4
#define BAR_CHAR5 0x5
#define DEGREE_CHAR 0x06

void LCD_prt (char x);
void LCD_cmd (char x);
void LCD_init (void);
void LCD_string(char *str);
void LCD_goto(int x, int y);
void LCD_clr();
void LCD_BL(int x);
bool LCD_IsBacklightOn(void);
void LCD_DrawBar(unsigned char percent);

void TESTER();


#endif
