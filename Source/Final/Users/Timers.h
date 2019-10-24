
#ifndef Timers_H
#define Timers_H

#include <Arduino.h>

enum Timetag
{
	Toff,
	T1s0,
	T2s0,
	T30s0 = 30,
	T60s0 = 60,
	T300s0 = 300,
};
class Timers
{
	public:
		Timers(unsigned long time);
		void  getMillis();
		void  settimer(uint16_t wTimer);
		void GetInitialTime(unsigned long timer);
		void GetTheTimeFromTheStart();
		void SetInitializationTimerFromData();
		bool  IsTimeout();
		unsigned long GetStartTimer();
		unsigned long GetTimerFromData();
	private:
		unsigned long timeout; 				// Tempo para Timeout
		unsigned long mulTimeCurrent;		// Tempo atual
		unsigned long mulTimeLast;			// Tempo do ultimo Timeout
		unsigned long mulItialTimer;		// Tempo do servidor NTP salvo
		unsigned long mulTimeFromTheStart; 	// Tempo deste que o micro está ativo utilizado 
											// para compensar o tempo entre a consulta ao 
											// servidor NTP e o msp responder que estpa ativo
		unsigned long mulTimerFromData;
		uint16_t mwTimer;
};
#endif
