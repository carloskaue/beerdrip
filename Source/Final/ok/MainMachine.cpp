#include "MainMachine.h"
#include "Timers.h"
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiClient.h>

#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>

#define APSSID "BeerDrip"
#define APPSK  "0123456789"


const char* wificonfig_file = "/wificonfig.json";
const char* beer_file = "/beer";
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


void handleRoot(void);
void handleForm(void);


// Objetos
Timers MainMachineTimer(1000);
Spiff fileWifConfig(wificonfig_files);
Spiff fileBeer(beer_file);
ESP8266WebServer server(80);
WiFiUDP udp;//Cria um objeto "UDP".
NTPClient timeClient(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.


MainMachine::MainMachine(uint8_t timeout )
{
	mudtMainMachineState=MM_INVALID;
	mudtMainMachineNextState=MM_INVALID;
	mbytMainMachineTimer = timeout;
}

void MainMachine::debugMainmachuine(MainMachineState state, EVENT event )
{
	String str = "State: ";

	switch (state)
	{
		case MM_Init:{str += "Init";}break;
		case MM_Init_SPIFF:{str += "Init_SPIFF";}break;
		case MM_Wifi_Conection:{str += "Wifi_Conection";}break;
		case MM_Wifi_Configuration:{str += "Wifi_Configuration" ;}break;
		default :{str +="default" ;}
	}
	str += ":\tEvent";
	switch(event)
	{
		case EVT_Constructor:{str += "Constructor";}break;
		case EVT_Destructor:{str += "Destructor";}break;
		case EVT_Timeout:{str += "Timeout";}break;
		default :{str +="default" ;}
	}
	Serial.println(str);
}

void MainMachine::setTimeout(uint8_t time)
{
	MainMachineTimer.settimer(time);
}

void MainMachine::begin(uint8_t time)
{
	setTimeout(time);
	mudtMainMachineNextState=MM_Init;
}

void MainMachine::setState(MainMachineState udtState)
{
	mudtMainMachineNextState = udtState;

}

void MainMachine::mainMachineEvents(EVENT udtEvent)
{
	
	debugMainmachuine(mudtMainMachineState,udtEvent);
	switch (mudtMainMachineState)
	{
///////////////////////////////////////////////////////
		
		case MM_Init:
		{
			switch (udtEvent)
			{
				case EVT_Constructor:
				{
					setTimeout(T1s0);
				}break;
				case EVT_Timeout:
				{
					//setState( MM_Init_SPIFF);
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
					fileWifConfig.begin();
					fileBeer.begin();
					setTimeout(T1s0);
				}break;
				case EVT_Timeout:
				{
					// Estancia um objeto Json
   					StaticJsonDocument<500> doc;

   					String json = fileWifConfig.ReadLine(0);

   					Serial.println(json);

   					if(json.legth() > 10)
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
					    serializeJson(doc,Serial);
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

						switch(LAST_CONECTION[0])
						{
							case 'F': {Serial.print("É igual a F");setState( MM_Wifi_Conection);}break;
							case 'T': {Serial.print("É igual a T"); setState( MM_Wifi_Configuration);}break;
							default: {Serial.print("Não é igual a nada");}
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

				}break;
			};
		}break;
	}

}

void MainMachine::mainLoop()
{
	if(mudtMainMachineState != mudtMainMachineNextState)
	{
		mainMachineEvents(EVT_Destructor);
		mudtMainMachineState = mudtMainMachineNextState;
		mainMachineEvents(EVT_Constructor);		
	}
	if(MainMachineTimer.IsTimeout())
	{
		mainMachineEvents(EVT_Timeout);	
	}
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
  writeFile(str, wificonfig_file, NEW_DOC);
  delay(100);
  ESP.restart();
}
