#ifndef MAIN_MACHINE_H
#define MAIN_MACHINE_H

#include <Arduino.h>

typedef enum tagMainMachineState
{
	MM_Init, 				// Inicialização do sistema
	MM_Init_SPIFF, 			// Inicialização dos arquivos
	MM_Wifi_Conection, 		// Conexão com o Wifi
	MM_Wifi_Configuration, 	// Configuração do Wifi
	MM_Init_NTP, 			// Configuração do servido NTP
	MM_Init_SPI, 			// Inicialização da comunicação com o MSP
	MM_Get_data_Msp,		// Pega os dados do Msp
	MM_Send_data,			// Envia os dados para o servidor

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
	void setTimeout(uint16_t time);
	void setState(MainMachineState udtState);
	void mainMachineEvents(EVENT udtEvent);
	
private:
	void debugMainmachuine(MainMachineState state, EVENT event );
	MainMachineState mudtMainMachineState;
	MainMachineState mudtMainMachineNextState;
	uint8_t mbytMainMachineTimer;
	uint8_t mbytAuxCont;
};
#endif
