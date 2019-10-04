
#ifndef Timers_H
#define Timers_H

#include <Arduino.h>
enum Timetag
{
	Toff,
	T1s0,
	T2s0,
	T30s0 = 30,
};
class Timers
{
public:
	Timers(unsigned long time);

	void  getMillis();
	void  settimer(uint8_t bytTimer);
	bool  IsTimeout();

private:
	unsigned long timeout;
	unsigned long mulTimeCurrent;
	unsigned long mulTimeLast;
	uint8_t mbytTimer;
};
#endif
