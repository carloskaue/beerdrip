typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t ULONG;

typedef enum tagMainMachineState
{
	MM_Init,
	MM_Init_SPIFF,
	MM_Wifi_Conection,
	MM_Wifi_Configuration,


	MM_INVALID

}MainMachineState;


typedef enum tagEVENT
{
	EVT_Constructor,
	EVT_Destructor,
	EVT_Timeout,
}EVENT;


enum TIMER
{
	T2s0 = 2,
};


MainMachineState mudtMainMachineState;
MainMachineState mudtMainMachineNextState;
BYTE mbytMainMachineTimer;

// Timer: Auxiliary variables
ULONG mulTimeCurrent = millis();
ULONG mulTimeLast = 0;

const char* PASSWORD_SSID;
const char* SSID;
const char* LAST_CONECTION;
const char* EMAIL;
const char* PASSWORD_EMAIL;
const char* TOKEN;
const char* wificonfig_file = "/wificonfig.json";
const char* beer_file = "/beer";


void SetTime(BYTE bytTimer);
void MainMachineLoop (void);
void MainMachine(EVENT udtEvent);

void MainMachine_Inic(void)
{
	mudtMainMachineState = MM_INVALID;
	MainMachine_SetState(MM_Init);
}

void MainMachineLoop (void)
{
	if(mudtMainMachineState != mudtMainMachineNextState)
	{
		MainMachine(EVT_Destructor);

		mudtMainMachineState = mudtMainMachineNextState;
		
		MainMachine(EVT_Constructor);
	}
}

void MainMachine_SetState(MainMachineState udtState)
{
	mudtMainMachineNextState = udtState;

}
void MainMachine(EVENT udtEvent)
{
	switch (mudtMainMachineState)
	{
		case MM_Init:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					SetTime(T2s0);
				}break;
				case EVT_Timeout:
				{
					mudtMainMachineNextState = MM_Init_SPIFF;
				}break;
			};
		}break;
		case MM_Init_SPIFF:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					SPIFF_SetStateMachine(SS_Inic);

				}break;
				case EVT_Timeout:
				{
					mudtMainMachineNextState = MM_Init_SPIFF;
				}break;
			};
		}break;
	}
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
				MainMachine(EVT_Timeout);
			}
		}

	}
}

void SetTime(BYTE bytTimer)
{
	mbytMainMachineTimer = bytTimer;
}


void setup()
{

}

void loop()
{

}