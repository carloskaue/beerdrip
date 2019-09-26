/**
  * @desc inicializa o sistema de arquivos
*/ bool openFS(void){
  //Abre o sistema de arquivos
  if(SPIFFS.begin())
  {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
    return true;
  }
    Serial.println("\nErro ao abrir o sistema de arquivos\n\n");
    return false;
}
/**
  * @desc lê conteúdo de um arquivo
  * @param string path - arquivo a ser lido
  * @return string - conteúdo lido do arquivo
*/
String readFile(String path) {
  File rFile = SPIFFS.open(path,"r");
  if (!rFile) 
  {
    Serial.println("Erro ao abrir arquivo para ler!");
    return "";
  }
  String content = rFile.readStringUntil('\r'); //desconsidera '\r\n'
  rFile.close();
  return content;
}

bool creadFile(String path)
{
  File wFile;

  wFile = SPIFFS.open(path, "w+");

	if(!wFile)
	{
		Serial.println("Erro ao criar o arquivo");
		return false;
	}
	else
	{
		Serial.println("Arquivo criado");
		return true;
	}
}

/**
  * @desc escreve conteúdo em um arquivo
  * @param string state - conteúdo a se escrever no arquivo
  * @param string path - arquivo a ser escrito
*/
bool writeFile(String state, String path, char* str) { 
	//Abre o arquivo para escrita ("w" write)
	//Sobreescreve o conteúdo do arquivo
	File rFile = SPIFFS.open(path,str); 
	if(!rFile)
	{
		Serial.println("Erro ao abrir arquivo para escrever!");
		return false
	}
	else
	{
		rFile.println(state);
	}
	rFile.close();
	return true;
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