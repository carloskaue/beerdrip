/*
    SPI Safe Master Demo Sketch
    Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   msp
   ===================================
     15       D8       SS   |   P4.4   preto
     13       D7      MOSI  |   P4.6   vermelho
     12       D6      MISO  |   P4.7   rosa
     14       D5      SCK   |   P4.5   marrom

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    This sketch tries to go around this issue by only pulsing the Slave Select line to reset the command
    and keeping the line LOW all other time.

*/
#include <SPI.h>
#define  _ss_pin SS

void setDataBits(uint16_t bits) {
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}

void setup() 
{
	Serial.begin(115200);
	SPI.begin();
	//SPI.setFrequency(16384);
    SPI.beginTransaction(SPISettings(16384, LSBFIRST, SPI_MODE2));

    pinMode(_ss_pin, OUTPUT);
    digitalWrite(_ss_pin, HIGH);
    setDataBits(8);
}


void loop() 
{   
	digitalWrite( _ss_pin , HIGH ) ; // Habilita o SS.
	delay(10);
	while(SPI1CMD & SPIBUSY) {}
	SPI1W0 = 0x02;
	SPI1CMD |= SPIBUSY;
	while(SPI1CMD & SPIBUSY) {}

	Serial.print((uint8_t)(SPI1W0&0xFF), HEX);
	Serial.print(" ");	
	
	SPI1W0 = 0xA0;
	SPI1CMD |= SPIBUSY;
	while(SPI1CMD & SPIBUSY) {}

	Serial.print((uint8_t)(SPI1W0&0xFF), HEX);
	Serial.print(" ");	
	
	SPI1W0 = 0x03;
	SPI1CMD |= SPIBUSY;
	while(SPI1CMD & SPIBUSY) {}

	Serial.print((uint8_t)(SPI1W0&0xFF), HEX);
	Serial.print("\n");	
	
	
	digitalWrite( _ss_pin , LOW ) ; // Desabilita o SS.   
	delay(500);     
}
