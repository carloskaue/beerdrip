#ifndef MAIN_MACHINE_H
#define MAIN_MACHINE_H

#include <Arduino.h>

typedef enum tagMainMachineState
{
	MM_Init,
	MM_Init_SPIFF,
	MM_Wifi_Conection,
	MM_Wifi_Configuration,
	MM_Init_NTP,


	MM_INVALID

}MainMachineState;

typedef enum tagEVENT
{
	EVT_WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    EVT_WL_IDLE_STATUS      = 0,
    EVT_WL_NO_SSID_AVAIL    = 1,
    EVT_WL_SCAN_COMPLETED   = 2,
    EVT_WL_CONNECTED        = 3,
    EVT_WL_CONNECT_FAILED   = 4,
    EVT_WL_CONNECTION_LOST  = 5,
    EVT_WL_DISCONNECTED     = 6,

	EVT_Constructor,
	EVT_Destructor,
	EVT_Timeout,
	EVT_WIFI_Conected,
	EVT_WIFI_Desconected,
}EVENT;

class MainMachine
{
public:
	MainMachine(uint8_t ime);
	void mainLoop();
	void begin(uint8_t time);
	void setTimeout(uint8_t time);
	void setState(MainMachineState udtState);
	void mainMachineEvents(EVENT udtEvent);
private:
	void debugMainmachuine(MainMachineState state, EVENT event );
	//void handleForm();
	//void handleRoot();
	MainMachineState mudtMainMachineState;
	MainMachineState mudtMainMachineNextState;
	uint8_t mbytMainMachineTimer;
	uint8_t mbytAuxCont;
};
#endif
