#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "FileConfig.h"


FileConfig wificonfig("/wificonfig.json");
FileConfig tabmesures("/tabmesures.json");
ESP8266WebServer server ( 80 );

void sendTabMesures() {
  server.send(200, "application/json", tabmesures.ToString());
  Serial.println("Tableau mesures envoyees");
}

void setup() {
  String string;
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;
  
  wificonfig.begin();
  tabmesures.begin();

  StaticJsonDocument<1024> doc;

  deserializeJson(doc, (string));
      const char* ssid = doc["wifi"]["password"];
    const char* password = doc["wifi"]["ssid"];
  //serializeJson(doc, Serial);
  // Start a new line
  Serial.println();
  string = "";

  WiFi.begin ( ssid, password);
  // Attente de la connexion au réseau WiFi / Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  // Connexion WiFi établie / WiFi connexion is OK
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid);
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
   
   server.on("/tabmesures.json", sendTabMesures);
   server.on("/action_page", handleForm); //form action is handled here
  /*HTTP_POST, []() {
    updateGpio();
  });
  */
  server.serveStatic("/js", SPIFFS, "/js");
  server.serveStatic("/css", SPIFFS, "/css");
  server.serveStatic("/img", SPIFFS, "/img");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/Config", SPIFFS, "/Config.html");

  server.begin();
  Serial.println ( "HTTP server started" );

}

void loop() {
  // not used in this example
    // put your main code here, to run repeatedly:
  server.handleClient();
  t++;
  if(t==0) h++;
  if(h==0) pa++;
  delay(100);
}
