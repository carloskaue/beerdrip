/*
    SPI Safe Master Demo Sketch
    Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   msp
   ===================================
     15       D8       SS   |   -
     13       D7      MOSI  |   P1.7
     12       D6      MISO  |   P1.6
     14       D5      SCK   |   P1.5

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    This sketch tries to go around this issue by only pulsing the Slave Select line to reset the command
    and keeping the line LOW all other time.

*/
#include <SPI.h>
#define  _ss_pin SS
unsigned char Txdata = 1;
unsigned char Rxdata = 0;

String str;

void setup() 
{
	Serial.begin(115200);
	SPI.begin();
	SPI.setFrequency(16384);
    SPI.beginTransaction(SPISettings(16384, MSBFIRST, SPI_MODE3));
    str = "";
	Serial.println();
    pinMode(_ss_pin, OUTPUT);
    digitalWrite(_ss_pin, HIGH);
}

void loop() 
{
   
    str += SPI.transfer(0x02);
    str+=" ";
    str += SPI.transfer(0xA1);
    str+=" ";
    str += SPI.transfer(0x03);
    Serial.println(str);

    str = "";
    
	delay(2000);
     
}