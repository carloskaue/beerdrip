#include "Timer.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include "MyDefines.h"

// Timer: Auxiliary variables
unsigned long mulTimeCurrent;
unsigned long mulTimeLast;
uint8_t mbytMainMachineTimer;

void Timers_Inic(void)
{
	mbytMainMachineTimer = 0;
	mulTimeCurrent = millis();
	mulTimeLast = 0;
}

void TimerLoop(void)
{
	// Current time
	mulTimeCurrent = millis();

	//Verifica se passou 1segundo
	if(mulTimeCurrent - mulTimeLast > 1000)
	{
		// Passou 1 segundo
		mulTimeLast = mulTimeCurrent;
		if(mbytMainMachineTimer)
		{
			if(--mbytMainMachineTimer)
			{
				//MainMachine(EVT_Timeout);
			}
		}

		Serial.print("1");

	}
}

void SetTime(uint8_t bytTimer)
{
	mbytMainMachineTimer = bytTimer;
}