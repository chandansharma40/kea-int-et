#ifndef LCD_H_
#define LCD_H_

void LCD_prt (char x);
void LCD_init (void);
void LCD_string(char *str);
void LCD_goto(int x, int y);
void LCD_clr();
void LCD_BL(int x);


#endif
