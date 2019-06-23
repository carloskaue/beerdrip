#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJsodn.h>

const char* wificonfig_file = "/wificonfig.txt";
File f;
void createFile(String path)
{
  File wFile;
  // Cria o arquivo caso ele não exita.
  if(SPIFFS.exists(path))
  {
    Serial.println("Arquivo existe");
  }
  else
  {
    Serial.println("Criando o arquivo");
    wFile = SPIFFS.open(path, "w+");
    if(!WFile)
    {
      Serial.println("Erro ao criar o arquivo, reiniciando o esp");
      ESP.restar();
    }
    else
    {
      Serial.println("Arquivo criado com sucesso");
    }
  }
  wFile.close();
}
/**
  * @desc escreve conteúdo em um arquivo
  * @param string state - conteúdo a se escrever no arquivo
  * @param string path - arquivo a ser escrito
*/
void writeFile(String state, String path) { 
  //Abre o arquivo para escrita ("w" write)
  //Sobreescreve o conteúdo do arquivo
  File rFile = SPIFFS.open(path,"w+"); 
  if(!rFile){
    Serial.println("Erro ao abrir arquivo!");
  } else {
    rFile.println(state);
    Serial.print("gravou estado: ");
    Serial.println(state);
  }
  rFile.close();
}
 
/**
  * @desc lê conteúdo de um arquivo
  * @param string path - arquivo a ser lido
  * @return string - conteúdo lido do arquivo
*/
String readFile(String path) {
  File rFile = SPIFFS.open(path,"r");
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  String content = rFile.readStringUntil('\r'); //desconsidera '\r\n'
  Serial.print("leitura de estado: ");
  Serial.println(content);
  rFile.close();
  return content;
}
 
/**
  * @desc inicializa o sistema de arquivos
*/ void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    Serial.println("\nErro ao abrir o sistema de arquivos");
  } else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}





void setup() 
{
  // Inicializa a seril
  Serial.begin(115200);
  while (!Serial) continue;


// Estancia um objeto Json
  StaticJsonDocument<200> doc;


  openFS();

// verifica se o arquivo existe
  if(SPIFFS.exists(wificonfig_file))
  {
    // O arquivo existe
    ////////////////////

    Serial.println("O arquivo existe");

    // Abre como leitura
    f = SPIFFS.open(wificonfig_file, "r");

   if (!f) 
    {
      // Não abriu o arquivo
      //////////////////////

      // sinaliza que não abriu o arquivo
      Serial.println("não abriu o arquivo");
    }

  }
  else
  {
    // O arquivo não existe 
    ///////////////////////
    createFile(wificonfig_file);

    writeFile("{\"wifi\":{\"ssid\":\"KAUE\",\"password\":\"cadu2011\"}}", wificonfig_file);
    Serial.print("Reiniciando ESP");
    ESP.restar();


  }
 
    const char* ssid = doc["wifi"]["password"];
    const char* password = doc["wifi"]["ssid"];


  // Print values.
  Serial.println("\n");
  Serial.println(ssid);
  Serial.println(password);
  serializeJsonPretty(doc, f);
}

void loop() {
  // not used in this example
}

