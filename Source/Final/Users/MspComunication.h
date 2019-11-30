
#ifndef USER_SPI_H
#define USER_SPI_H

#include <Arduino.h>
#include <Spiff.h>


#define  beer_file  "/beer"

extern Spiff fileBeer;

enum SPI_Command
{
	SC_Starting = 0xA0,
	SC_Consumption_Request = 0xA5,
};


class MspComunication
{
	public:
		MspComunication(uint16_t wFrequency);
		void begin(uint16_t bits);
		bool getMSPStart(void);					// Faz a requição para marcar o inicio do funcionamento do msp
		bool getDataBeer(void);					// Faz a requisição para pegar os dados do msp.


	private:
		uint16_t mwFrequency;
		uint8_t transfer(uint8_t data);
};
#endif