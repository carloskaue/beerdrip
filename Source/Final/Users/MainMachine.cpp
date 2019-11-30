#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiClient.h>
#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>

#include <MainMachine.h>
#include <Timers.h>
#include <Spiff.h>
#include <MspComunication.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
const char fingerprint[] PROGMEM = "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30";
//Web/Server address to read/write from 
const char *host = "https://forcraft-staging.herokuapp.com/api/v1/token"; 
const char *host2 = "https://forcraft-staging.herokuapp.com/api/v1/consumptions"; 


#define APSSID "BeerDrip"
#define APPSK  "0123456789"

#define TIME_WIFI_CONECTION T30s0

wl_status_t mudtWifiState;
wl_status_t mudtWifiNexState;

const char* PASSWORD_SSID;
const char* SSID;
const char* LAST_CONECTION;
const char* EMAIL;
const char* PASSWORD_EMAIL;
const char* TOKEN;
const char* wificonfig_file = "/wificonfig.json";
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<h2>Circuits4you<h2>
<h3> HTML Form ESP8266</h3>

<form action="/action_page">
  SSID:<br>
  <input type="text" name="SSID" value="Mickey">
  <br>
  PASSWORD_SSID:<br>
  <input type="text" name="PASSWORD_SSID" value="Mouse">
  <br><br>
  <input type="submit" value="Submit">
</form> 

</body>
</html>
)=====";
const char* resultPayload = "{ \"message\": \"sucess\" }";

void handleRoot(void);
void handleForm(void);

//////////////
// Objetos
//////////////

// Timer
Timers timerMainMachine(1000);
// Arquivo da configuração do wifi
Spiff fileWifConfig(wificonfig_file);
// Comunicação SPI
MspComunication mspComunication(16384);
// Para configuração do Wifi
ESP8266WebServer server(80);
// Para a conexão com o servidos
WiFiUDP udp;//Cria um objeto "UDP".
// Para pegar a hora de referencia dos dados
NTPClient timerNTP(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.

// Estancia um objeto Json
StaticJsonDocument<500> doc;


MainMachine::MainMachine(uint8_t timeout )
{
	mudtMainMachineState=MM_INVALID;
	mudtMainMachineNextState=MM_INVALID;
	mbytMainMachineTimer = timeout;
	mudtWifiState = WL_IDLE_STATUS;
	mudtWifiNexState = WL_IDLE_STATUS;
}

// Debug da mainmachine
void MainMachine::debugMainmachuine(MainMachineState state, EVENT event )
{
	String str = "State: ";

	switch (state)
	{
		case MM_Init:{str += "Init";}break;
		case MM_Init_SPIFF:{str += "Init_SPIFF";}break;
		case MM_Wifi_Conection:{str += "Wifi_Conection";}break;
		case MM_Wifi_Configuration:{str += "Wifi_Configuration" ;}break;
		case MM_Init_NTP:{str +="Init_NTP";}break;
		case MM_Init_SPI:{str +="Init_SPI";}break;
		case MM_Get_data_Msp:{str +="Get_data_Msp";}break;
		case MM_Send_data:{str +="Send_data";}break;
	}
	str += ":\tEvent: ";
	switch(event)
	{
	case EVT_WL_NO_SHIELD :  {str += "NO_SHIELD";} break;    
    case EVT_WL_IDLE_STATUS:    {str += "IDLE_STATUS";}  break;
    case EVT_WL_NO_SSID_AVAIL:    {str += "NO_SSID_AVAIL";}break;
    case EVT_WL_SCAN_COMPLETED:  {str += "SCAN_COMPLETED";}break;
    case EVT_WL_CONNECTED:        {str += "CONNECTED";}break;
    case EVT_WL_CONNECT_FAILED:  {str += "CONNECT_FAILED";} break;
    case EVT_WL_CONNECTION_LOST:{str += "CONNECTION_LOST";}break;
    case EVT_WL_DISCONNECTED:{str += "DISCONNECTED";}break;
	case EVT_Constructor:{str += "Constructor";}break;
	case EVT_Destructor:{str += "Destructor";}break;
	case EVT_Timeout:{str += "Timeout";}break;
	case EVT_WIFI_Conected:{str += "WIFI_Conected";}break;
	case EVT_WIFI_Desconected:{str += "WIFI_Desconected";}break;
	}
	Serial.println(str);
}

// Define o tempo para timeout
void MainMachine::setTimeout(uint16_t time)
{
	timerMainMachine.settimer(time);
}

// inicializa
void MainMachine::begin(uint8_t time)
{
	setTimeout(time);
	mudtMainMachineNextState=MM_Init;
}
// Define o novo estado
void MainMachine::setState(MainMachineState udtState)
{
	mudtMainMachineNextState = udtState;
}

// MainMachine eventos
void MainMachine::mainMachineEvents(EVENT udtEvent)
{
	
	debugMainmachuine(mudtMainMachineState,udtEvent);
	switch (mudtMainMachineState)
	{
///////////////////////////////////////////////////////
		// Inicialização do sistema
		// Espera 1 segundo
		case MM_Init:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					// Define o timeout para 1 segundo
					setTimeout(T1s0);
				}break;
				case EVT_Timeout:
				{
					// Vai para o estado para iniciar o sistema de memoria dos arquivos
					setState( MM_Init_SPIFF);
				}break;
			};
		}break;
		
///////////////////////////////////////////////////////
		
		// Inicialização do sistema de memoria dos arquivos
		case MM_Init_SPIFF:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					// Inicializa os arquivos

					// Arquivo de configuração do wifi
					fileWifConfig.begin();
					
					// Timeout 1segundo
					setTimeout(T1s0);
				}break;
				case EVT_Timeout:
				{
					// le o arquivo de configuração
					String json = fileWifConfig.ReadLine(0);

   					if(json.length() > 10)
   					{
   						DeserializationError error = deserializeJson(doc, json);
						// Test if parsing succeeds.
					    if (error) 
					    {
					      Serial.print("deserializeJson() failed: ");
					      Serial.println(error.c_str());

					      //TODO: case de erro voltar para as confifurações de fabrica
					      //return;
					    }
					    else
					    {
						    //serializeJson(doc,Serial);
							SSID           = doc["wifi"]["SSID"];
							PASSWORD_SSID  = doc["wifi"]["PASSWORD_SSID"];
							LAST_CONECTION = doc["wifi"]["LAST_CONECTION"];
							EMAIL          = doc["authentication"]["email"];
							PASSWORD_EMAIL = doc["authentication"]["password"];    
							TOKEN          = doc["authentication"]["token"];

							switch(LAST_CONECTION[0])
							{
								case 'F': {setState(MM_Wifi_Configuration );}break;
								case 'T': {setState(MM_Wifi_Conection );}break;
								default:{}
							}  

						}
	   				}
	   				else
	   				{
	   					 setState( MM_Wifi_Configuration);
	   				}					
				}break;
			};
		}break;

///////////////////////////////////////////////////////
		
		// Configuração para conexação do Wifi
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

		// Tenta conectar no Wifi com o ssid e senha salvo na memoria
		case MM_Wifi_Conection:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					// Inicia a conexão
					setTimeout(TIME_WIFI_CONECTION);
 					
 					WiFi.softAPdisconnect();
					WiFi.disconnect();
					WiFi.mode(WIFI_STA);
					WiFi.begin(SSID, PASSWORD_SSID);

				}break;
				case EVT_WL_CONNECT_FAILED:
				case EVT_WL_CONNECTION_LOST:
				case EVT_WL_DISCONNECTED:
				{
					//TODO: Nada?
				}
				break;
				case EVT_WL_CONNECTED:
				{
					Serial.println("");
					Serial.println("WiFi connected");
					Serial.println("IP address: ");
					Serial.println(WiFi.localIP());
					setState(MM_Init_NTP);
				}
				break;
				case EVT_Timeout:
				{
					// Falha na conexão 
					String str = "{\"wifi\":{\"SSID\":\"";
					str += SSID;
					str += "\",\"PASSWORD_SSID\":\"";
					str += PASSWORD_SSID;
					str += "\",\"LAST_CONECTION\":\"F\"}}";
					fileWifConfig.Write(str);
					ESP.restart();
					
				}
				break;
			};
		}break;

//////////////////////////////////////////////////////////////

		// Conexão ao servidor NTP
		case MM_Init_NTP:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					//Inicializa o client NTP
    				timerNTP.begin();
    				setTimeout(T1s0);

				}break;
				case EVT_Timeout:
				{
					
					// Verifica se teve um update
					if(!timerNTP.update())
					{
						// Não deve um update

						//Força um update 
						timerNTP.forceUpdate();
					}

					
					Serial.print("Initial timer: ");
					Serial.println(timerNTP.getFormattedTime());
					// Salva a hora 
					timerMainMachine.GetInitialTime(timerNTP.getEpochTime());
					// Salva o tempo que o micro está ativado.
					timerMainMachine.GetTheTimeFromTheStart();
					// Fecha a conexão
					timerNTP.end();
					
					//  Estado que inicia a comunicação SPI
					setState(MM_Init_SPI);
				}
			}

		}break;
//////////////////////////////////////////////////////////////////////////////
		// Inicia a comunicação com o MSP
		case MM_Init_SPI:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{				
					// Configura a comunicação SPI	
					mspComunication.begin(8);

					setTimeout(T1s0);					
				}break;

				case EVT_Timeout:
				{
					//TODO: Oque fazer caso o MSP nao responder?
					// Verifica se o MSP está respondendo
					 digitalWrite(SS, HIGH);
      				delayMicroseconds(5);
					if(mspComunication.getMSPStart())
					{
						// Respondeu
						////////////
 						digitalWrite(SS, LOW);
						// Salva a hora que o MSP respondeu.
						timerMainMachine.SetInitializationTimerFromData();

						setState(MM_Get_data_Msp);
					}
					else
					{
						// Alguma coisa deu errado, espera mais 1 segundo para tentar
						// a comunicação novamente.
						setTimeout(T1s0);
					}	
				}
				break;
			}
		}
///////////////////////////////////////////////////////////////////////////////////////
		case MM_Get_data_Msp:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					// Espera 5 minutos para requisitar os dados do msp
					setTimeout(T1s0);
				}break;
				case EVT_Timeout:
				{
					// Faz a requisição dos dados
					 digitalWrite(SS, HIGH);
      				delayMicroseconds(5);
					// Verifica se houve a comunicação.
					if(mspComunication.getDataBeer())
					{

						// Garante o inicio do arquivo.
						fileBeer.ResetPosition();

						HTTPClient http; 
        				http.begin (host, "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30"); 

						// Fica preso tentando enviar os dados
						// TODO: Fazer oq caso não consiga por 'n' motivos?
						while(fileBeer.AlreadyReachedEndOfFile()==false && WiFi.status()== WL_CONNECTED)
						{
							http.addHeader("Content-Type", "application/json");
							http.addHeader("Authorization", "eyJhbGciOiJIUzI1NiJ9.eyJ1c2VyX2lkIjoyfQ.JpqTQAbNqTQrzw1F5QbhS0sstFZSWG30OPe0AM7WiwA");
							// Pega a linha pra ser enviada
							int httpCode = http.POST(fileBeer.ReadLineToSend());
							if(httpCode == 201)
							{
							
								// Pega o resultado do envio
								String payload = http.getString();
								// Verifica se deu certo
								if(payload.equals(resultPayload))
								{
									// MSg enviada
									///////////////
									fileBeer.SetNewPosition();

									// podeir para a proxima linha
								}
								// else
								// {
								// 	// Msg não enviada
								// 	///////////////////

								// 	// Tenta reenviar
								// }
							
								Serial.println(payload);  
							}
						}      
						http.end();
						if(fileBeer.AlreadyReachedEndOfFile())
						{
							fileBeer.RemoveFile();
						}
					}
						 digitalWrite(SS, LOW);
      					// Por algum motivo não houve a comunicação
						// Tenta novamente em 5minutos
						// Ou se finalizar o envio de dados esperar 
						// os 5 minutos realizar novamente o envio dos novos dados.
						setTimeout(T300s0);
					
				}			
			default:
				break;
			}
		}
//////////////////////////////////////////////////////////////////////////
	}

}

// Controle da troca de estado, gerador de eventos do wifi e controle do timeout
void MainMachine::mainLoop()
{
	//////////////////////////////////////////////////
	// Atualização do estados da main machine
	/////////////////////////////////////////////////
	if(mudtMainMachineState != mudtMainMachineNextState)
	{
		mainMachineEvents(EVT_Destructor);
		mudtMainMachineState = mudtMainMachineNextState;
		mainMachineEvents(EVT_Constructor);		
	}
	//////////////////////////////////////////////////
	// Atualização do estados da conexão do Wifi
	/////////////////////////////////////////////////

	mudtWifiNexState = WiFi.status();
	if(mudtWifiState != mudtWifiNexState)
	{
		mudtWifiState = mudtWifiNexState;
		mainMachineEvents((EVENT)mudtWifiState);	
	}

	// Controle do Timeout da MainMachine
	if(timerMainMachine.IsTimeout())
	{
		mainMachineEvents(EVT_Timeout);	
	}
	server.handleClient();
}



void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleForm() 
{
  String ssid = server.arg("SSID"); 
  String password_ssid = server.arg("PASSWORD_SSID"); 

  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.print("PASSWORD_SSID: ");
  Serial.println(password_ssid);

  String str = "{\"wifi\":{\"SSID\":\"";
  str += ssid;
  str += "\",\"PASSWORD_SSID\":\"";
  str += password_ssid;
  str += "\",\"LAST_CONECTION\":\"T\"}}";

  String s = "<a href='/'> Go Back </a>";
  server.send(200, "text/html", s); //Send web page

  fileWifConfig.Write(str);
  
  delay(100);
  
  ESP.restart();
}
