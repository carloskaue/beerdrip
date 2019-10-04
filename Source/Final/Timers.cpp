#include"Timers.h"

Timers::Timers(unsigned long bytTimer)
{
	timeout = bytTimer;   
	mulTimeCurrent = 0;
	mulTimeLast = millis();
	mbytTimer = 0;
}


void Timers::getMillis()
{
   mulTimeCurrent = millis();
}
void Timers::settimer(uint8_t bytTimer)
{
   mbytTimer = bytTimer;
}

bool Timers::IsTimeout()
{
	getMillis();

		//Verifica se passou 1segundo
	if(mulTimeCurrent - mulTimeLast >= timeout)
	{
		// Passou 1 segundo
		mulTimeLast = mulTimeCurrent;
		if(mbytTimer)
		{
			if(--mbytTimer==0)
			{
				return true;
			}
		}
	}
	return false;
}
