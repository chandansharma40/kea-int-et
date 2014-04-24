#ifndef CLOCK_H_
#define CLOCK_H_



void TIME_Tick (void);
void timer1_init();

int TIME_GetSeconds (void);
int TIME_GetMinutes (void);
int TIME_GetHours (void);

void TIME_SetSeconds (int value);
void TIME_SetMinutes (int value);
void TIME_SetHours (int value);

#endif
