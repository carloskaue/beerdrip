#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>


#include <ArduinoJson.h>
#include "FileConfig.h"


FileConfig wificonfig("/wificonfig.json");
FileConfig tabmesures("/tabmesures.json");
const char* host = "esp8266fs";
ESP8266WebServer server(80);




void sendTabMesures() {
  server.send(200, "application/json", tabmesures.ToString());
  Serial.println("Tableau mesures envoyees");
}








void setup() {
  String string ="";
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;
  
  wificonfig.begin();
  string = wificonfig.ToString() ;
  StaticJsonDocument<1024> wifi;
  StaticJsonDocument<1024> table;

  deserializeJson(wifi, string);
  const char* ssid = wifi["ssid"];
  const char* password = wifi["password"];

  string = tabmesures.ToString() ;
  deserializeJson(table, string);

   //WIFI INIT
  Serial.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.begin(host);
  Serial.print("Open http://");
  Serial.print(host);
  Serial.println(".local/edit to see the file browser");

  

}

void loop() {
  // not used in this example
    // put your main code here, to run repeatedly:
  delay(100);
}
