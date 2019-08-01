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

const char* password;
const char* ssid;
const char* LastConection;
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
  PASSWORD:<br>
  <input type="text" name="PASSWORD" value="Mouse">
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
  WiFi.begin(ssid, password);
  
  uint8 byteRetentativas = 0;

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    if(++byteRetentativas>=30)
    {
      Serial.println("Conecção Falha");


      String str = "{\"wifi\":{\"ssid\":\"";
      str += ssid;
      str += "\",\"password\":\"";
      str += password;
      str += "\",\"LastConection\":\"F\"}}";
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
  String password = server.arg("PASSWORD"); 

  Serial.print("ssid: ");
  Serial.println(ssid);

  Serial.print("password: ");
  Serial.println(password);

  String str = "{\"wifi\":{\"ssid\":\"";
  str += ssid;
  str += "\",\"password\":\"";
  str += password;
  str += "\",\"LastConection\":\"T\"}}";

  String s = "<a href='/'> Go Back </a>";
  server.send(200, "text/html", s); //Send web page
  writeFile(str, wificonfig_file, NEW_DOC);
  delay(100);
  ESP.restart();
}

void CreatWifi(void)
{  

  /* You can remove the password parameter if you want the AP to be open. */
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
   StaticJsonDocument<150> doc;

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

    ssid = doc["wifi"]["ssid"];
    password = doc["wifi"]["password"];
    LastConection =  doc["wifi"]["LastConection"];
    Serial.println(LastConection);
    
    switch(LastConection[0])
    {
      case 'F': {Serial.print("É igual a F");CreatWifi();}break;
      case 'T': {Serial.print("É igual a T"); ConectToWifi();}break;
      default: {Serial.print("Não é igual a nada");}
    }    
   }
   else
  {
    Serial.println("O arquivo não existe");
    String str = "{\"wifi\":{\"ssid\":\"";
    str += APSSID;
    str += "\",\"password\":\"";
    str += APPSK;
    str += "\",\"LastConection\":\"T\"}}";
    writeFile(str, wificonfig_file, NEW_DOC);
   }

   setupNTP();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  timeClient.update();                              // atualiza o relogio
  Serial.print(timeClient.getEpochTime());
  Serial.print("\t");
  Serial.println(timeClient.getFormattedTime());    // print do relogio da WEB
  delay(1000);                                      // atraso de um segundo
}
