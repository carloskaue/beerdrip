#ifdef MAINMACHINE_H
#include "MyDefines.H"

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

void MainMachine_Inic(void)
void MainMachineLoop (void);
void MainMachine(EVENT udtEvent);
void MainMachine_SetState(MainMachineState udtState)

#endif //MAINMACHINE_H


