
enum TIMER
{
	T1s = 1,
	T2s = 2,
};



// Timer: Auxiliary variables
ULONG mulTimeCurrent = millis();
ULONG mulTimeLast = 0;



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
				MainMachine(EVT_Timeout);
			}
		}

	}
}

void SetTime(BYTE bytTimer)
{
	mbytMainMachineTimer = bytTimer;
}