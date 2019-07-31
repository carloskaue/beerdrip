/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
const char fingerprint[] PROGMEM = "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30";
//Web/Server address to read/write from 
const char *host = "https://forcraft-staging.herokuapp.com/api/v1/token"; 
const char *host2 = "https://forcraft-staging.herokuapp.com/api/v1/consumptions"; 



ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Kaue", "Cadu2011");
}

void loop () {

    if (WiFi.status () == WL_CONNECTED) 
    {// Verificar o status da conexão WiFi

        HTTPClient http; 
        http.begin (host, "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30"); 


        http.addHeader("Content-Type", "application/json");
        //http.addHeader("Authorization", "eyJhbGciOiJIUzI1NiJ9.eyJ1c2VyX2lkIjoyfQ.JpqTQAbNqTQrzw1F5QbhS0sstFZSWG30OPe0AM7WiwA");
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
        // COODE 201 POST OK
    }
    else{

        Serial.print("Error in Wifi connection");

    }

    delay(30000); //Send a request every 30 seconds

}