                                       #include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiClient.h>

#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>


#define NEW_LINE "a+"
#define NEW_DOC "w+"

#ifndef APSSID
#define APSSID "BeerDrip"
#define APPSK  "0123456789"
#endif


const char* PASSWORD_SSID;
const char* SSID;
const char* LAST_CONECTION;
const char* EMAIL;
const char* PASSWORD_EMAIL;
const char* TOKEN;
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

ESP8266WebServer server(80);

WiFiUDP udp;//Cria um objeto "UDP".
NTPClient timeClient(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.


void CreatWifi(void);

void setupNTP()
{
    //Inicializa o client NTP
    timeClient.begin();
     
    //Espera pelo primeiro update online
    Serial.println("Waiting for first update");
    while(!timeClient.update())
    {
        Serial.print(".");
        timeClient.forceUpdate();
        delay(500);
    }
 
    Serial.println();
    Serial.println("First Update Complete");
}

 /**
  * @desc inicializa o sistema de arquivos
*/ void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    Serial.println("\nErro ao abrir o sistema de arquivos\n\n");
  } else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}
/**
  * @desc lê conteúdo de um arquivo
  * @param string path - arquivo a ser lido
  * @return string - conteúdo lido do arquivo
*/
String readFile(String path) {
  File rFile = SPIFFS.open(path,"r");
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo para ler!");
  }
  String content = rFile.readStringUntil('\r'); //desconsidera '\r\n'
  rFile.close();
  return content;
}

void creadFile(String path)
{
  File wFile;

  if(SPIFFS.exists(path))
  {
     Serial.println("O arquivo existe");
  }
  else
  {
    Serial.println("Criando os arquivos...");

  }

  wFile = SPIFFS.open(path, "w+");

    if(!wFile)
  {
     Serial.println("Erro ao criar o arquivo");
  }
  else
  {
    Serial.println("Arquivo criado");

  }
}

/**
  * @desc escreve conteúdo em um arquivo
  * @param string state - conteúdo a se escrever no arquivo
  * @param string path - arquivo a ser escrito
*/
void writeFile(String state, String path, char* str) { 
  //Abre o arquivo para escrita ("w" write)
  //Sobreescreve o conteúdo do arquivo
  File rFile = SPIFFS.open(path,str); 
  if(!rFile){
    Serial.println("Erro ao abrir arquivo para escrever!");
  }
  else
  {
    rFile.println(state);
  }
  rFile.close();
}

void ConectToWifi(void)
{
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD_SSID);
  
  uint8 byteRetentativas = 0;

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    if(++byteRetentativas>=30)
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

  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
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

void CreatWifi(void)
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

}

void setup() 
{
  // Inicializa a seril
  Serial.begin(115200);
  while (!Serial) continue;

  // // Estancia um objeto Json
   StaticJsonDocument<500> doc;

   openFS();

  // // verifica se o arquivo existe
  if(SPIFFS.exists(wificonfig_file))
   {
    //   // O arquivo existe
    //   ////////////////////

    Serial.println("O arquivo existe");

    String json = readFile(wificonfig_file);
    
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
      case 'F': {Serial.print("É igual a F");CreatWifi();}break;
      case 'T': {Serial.print("É igual a T"); ConectToWifi();}break;
      default: {Serial.print("Não é igual a nada");}
    }    
   }
   else
  {
    Serial.println("O arquivo não existe");
    String str = "{\"wifi\":{\"SSID\":\"";
    str += APSSID;
    str += "\",\"PASSWORD_SSID\":\"";
    str += APPSK;
    str += "\",\"LAST_CONECTION\":\"T\"}}";
    writeFile(str, wificonfig_file, NEW_DOC);
   }

   setupNTP();
}

void SendData(void)
{
	if(SPIFFS.exists(beer_file))
	{
		File rFile = SPIFFS.open(beer_file,"r"); 

		Serial.print("Tamanho Arquivo:\t");
		Serial.print(rFile.size())
		rFile.close();
		/*
		 if (WiFi.status () == WL_CONNECTED) 
		 {
		 	HTTPClient http; 
        http.begin (host, "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30"); 
        http.addHeader("Content-Type", "application/json");



        String postMessage = "{\"consumptions\":[{\"resource\": \"t1010\",\"quantity\": 5,\"time\": 123}]}";//{\"email\":\"mandakeru@gmail.com\",\"password\":\"qweqwe123\"}";
        int httpCode = http.POST("{\"email\":\"admin@teste.com\",\"password\":\"qweqwe123\"}");
        Serial.print("http result:");
        // httpcode 200
        Serial.println(httpCode);
          //http.writeToStream(&Serial);
        String payload = http.getString();
        Serial.println(payload);        
        http.end();

        http.begin (host2, "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30"); 
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "eyJhbGciOiJIUzI1NiJ9.eyJ1c2VyX2lkIjoyfQ.JpqTQAbNqTQrzw1F5QbhS0sstFZSWG30OPe0AM7WiwA");
        httpCode = http.POST("{\"consumptions\":[{\"resource\": \"T010\",\"quantity\": 500,\"time\": 123}]}");
        Serial.print("http result:");
        //httpcode 201
        Serial.println(httpCode);
        payload = http.getString();
        Serial.println(payload);        
        http.end();

		 }
*/
	}
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  // atualiza o relogio
  timeClient.update();                              
  Serial.print(timeClient.getEpochTime());
  Serial.print("\t");
  // print do relogio da WEB
  Serial.println(timeClient.getFormattedTime());    
  delay(1000);                                      // atraso de um segundo



	SendData();



}
