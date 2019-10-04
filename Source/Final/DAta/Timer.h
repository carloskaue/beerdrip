#ifdef TIMER_H

#include "MyDefines.h"

enum TIMER
{
	T1s = 1,
	T2s = 2,
};


void Timers_Inic(void);
void TimerLoop(void);
void SetTime(uint8_t bytTimer);

#endif TIMER_H
