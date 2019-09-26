
typedef enum tagMainMachineState
{
	MM_Init,
	MM_Init_SPIFF,
	MM_Wifi_Conection,
	MM_Wifi_Configuration,


	MM_INVALID

}MainMachineState;

MainMachineState mudtMainMachineState;
MainMachineState mudtMainMachineNextState;
BYTE mbytMainMachineTimer;

BYTE mbytAuxCont;


void SetTime(BYTE bytTimer);
void MainMachineLoop (void);
void MainMachine(EVENT udtEvent);

void MainMachine_Inic(void)
{
	mudtMainMachineState = MM_INVALID;
	MainMachine_SetState(MM_Init);
	mbytAuxCont = 0;
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

///////////////////////////////////////////////////////
		
		case MM_Init:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					SetTime(T2s);
				}break;
				case EVT_Timeout:
				{
					mudtMainMachineNextState = MM_Init_SPIFF;
				}break;
			};
		}break;
		
///////////////////////////////////////////////////////
		
		case MM_Init_SPIFF:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					SPIFF_SetStateMachine(SS_Inic);
					SetTime(T2s0);

				}break;
				case EVT_Timeout:
				{
					// Houve algum problema
					// Esse timeout não poderia acontecer
					// Vamos reiniciar o dispositivo.					
					ESP.restart();
				}break;
			};
		}break;

///////////////////////////////////////////////////////
		
		case MM_Wifi_Configuration:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					/* You can remove the PASSWORD_SSID parameter if you want the AP to be open. */
					WiFi.softAP(APSSID, APPSK);

					IPAddress myIP = WiFi.softAPIP();
					Serial.print("AP IP address: ");
					Serial.println(myIP);
					server.on("/", handleRoot);      //Which routine to handle at root location
					server.on("/action_page", handleForm); //form action is handled here
					server.begin();
					Serial.println("HTTP server started");
				}break;
			};
		}break;

///////////////////////////////////////////////////////

		case MM_Wifi_Conection:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
						WiFi.softAPdisconnect();
						WiFi.disconnect();
						WiFi.mode(WIFI_STA);
						WiFi.begin(SSID, PASSWORD_SSID);
						mbytAuxCont = 0;
						SetTime(T1s);
				}break;

				case EVT_Timeout:
				{
					if(WiFi.status() == WL_CONNECTED)
					{
						Serial.println("");
						Serial.println("WiFi connected");
						Serial.println("IP address: ");
						Serial.println(WiFi.localIP());  

						// TODO:
						MainMachine_SetState();
					}
					else
					{
						if(++mbytAuxCont >= 30)
						{
							Serial.println("Conecção Falha");

							String str = "{\"wifi\":{\"SSID\":\"";
							str += SSID;
							str += "\",\"PASSWORD_SSID\":\"";
							str += PASSWORD_SSID;
							str += "\",\"LAST_CONECTION\":\"F\"}}";
							writeFile(str, wificonfig_file, NEW_DOC);
							ESP.restart();
						}
						else
						{
							SetTime(T1s);
						}

					}
				}
			};
		}break;
	}
}