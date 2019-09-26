typedef tagSpiffState
{
	SS_Inic,


	SS_Invalid;

}SpiffState;

SpiffState mudtSpiffState;
SpiffState mudtSpiffNextState;

void SPIFF_inic(void)
{
	mudtSpiffState = SS_Invalid;
	
	mudtSpiffNextState = SS_Invalid;
}

void SPIFF_SetStateMachine(SpiffState udtState)
{
	mudtSpiffNextState = udtState;
}

void SPIFF_mainloop(void)
{
	if(mudtSpiffState != mudtSpiffNextState)
	{
		SPIFF_Machine(EVT_Destructor);

		mudtSpiffState = mudtSpiffNextState

		SPIFF_Machine(EVT_Constructor);

	}
		
}

void SPIFF_Machine(EVENT udtEvt)
{
	switch(mudtSpiffState)
	{
		case SS_Inic:
		{
			switch(udtEvt)
			{
				case EVT_Constructor:
				{
					openFS();

					 // verifica se o arquivo existe
					if(SPIFFS.exists(wificonfig_file))
					{
						// O arquivo existe
						////////////////////

						Serial.println("O arquivo existe");

						// SSID 			32 bytes          
						// PASSWORD_SSID    64 bytes
						// LAST_CONECTION 	1 byte
						// EMAIL            100 bytes
						// PASSWORD_EMAIL   64 bytes
						// TOKEN            100 bytes
						// ---------------------------
						// Total            361 bytes
						StaticJsonDocument<400> doc;

						String json = readFile(wificonfig_file);

						DeserializationError error = deserializeJson(doc, json);
						// Test if parsing succeeds.
						if (error) 
						{
							Serial.print("deserializeJson() failed: ");
							Serial.println(error.c_str());

							//TODO: case de erro voltar para as confifurações de fabrica
							MainMachine_SetState(MM_Wifi_Configuration);
							return;
						} 
						serializeJson(doc,Serial);
						Serial.println();

						SSID           = doc["wifi"]["SSID"];
						PASSWORD_SSID  = doc["wifi"]["PASSWORD_SSID"];
						LAST_CONECTION = doc["wifi"]["LAST_CONECTION"];
						EMAIL          = doc["authentication"]["email"];
						PASSWORD_EMAIL = doc["authentication"]["password"];    
						TOKEN          = doc["authentication"]["token"];


						Serial.println("Dados da conexão\n************************");
						Serial.print("Ssid:\t");Serial.println(SSID          );
						Serial.print("Password:\t");Serial.println(PASSWORD_SSID );
						Serial.print("Ultima conexão:\t");Serial.println(LAST_CONECTION);
						Serial.print("Email:\t");Serial.println(EMAIL         );
						Serial.print("Password:\t");Serial.println(PASSWORD_EMAIL);
						Serial.print("Token:\t");Serial.println(TOKEN         );
						Serial.println("************************");

						Serial.print()

						switch(LAST_CONECTION[0])
						{
							// Ultima inicialização houve uma conexão no WIfi
							case 'T': 
							{
								MainMachine_SetState(MM_Wifi_Conection);
							}break;
							// Ultima inicialização não houve uma conexão no wifi
							// habilita a configuração do wifi
							case 'F': 
							default:
							{
								MainMachine_SetState(MM_Wifi_Configuration);
							}
						}    
					}
					else
					{
						// O arquivi de configuração não existe
						////////////////////////////////////////

						// Habilita a configuração do wifi
					    MainMachine_SetState(MM_Wifi_Configuration);
					}

				}break;
			}
		}break;
	}

}