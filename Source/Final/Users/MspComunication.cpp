#include <MspComunication.h>
#include <SPI.h>



const uint8_t FrameStartig[3] 			= {0x02, 0xA1, 0x03};
const uint8_t FrameConsumptionRequest[3] 	= {0x02, 0xA5, 0x03};

// Arquivo dos dados
Spiff fileBeer(beer_file);

MspComunication::MspComunication(uint16_t wFrequency)
{
	mwFrequency = wFrequency;
}

void MspComunication::begin(void)
{
	SPI.begin();
	SPI.beginTransaction(SPISettings(wFrequency, MSBFIRST, SPI_MODE1));

	// Aquivo dos dados
	fileBeer.begin();
}
uint8_t MspComunication::transfer(uint8_t data)
{
	SPI.transfer(data);
}

//TODO: método para verificar a mensagem recebida bate com a esperada.
bool MspComunication::getMSPStart(void)
{
	String str = "";
	unsigned char data;

	for (uint8_t indice = 0; indice < 3; indice ++)
	{
		transfer(FrameStartig[indice]);
	}
	// while(data != 0x03)
	// {
	// 	data = transfer(0);
	// 	str += data;
	// 	if(data == 0x03)
	// 	{
	// 		Serial.println(str);
	// 	}
	// }
	return true;
}

//TODO: fazer metodo para pegar os dados do msp
bool MspComunication::getDataBeer()
{
	uint8_t data = 0;
	String str = "";
	for (uint8_t indice = 0; indice < 3; indice ++)
	{
		transfer(FrameStartig[indice]);
	}
	
	while(data != 0x03)
	{
		data = transfer(0);
		if(data != 0x02 || data != 0x03)
		{
			str += data;
			if(data  = '\n')
			{
				Serial.print(str);
				if(fileBeer.Writeline(str))
				{
					str = "";
				}
			}
		}
	}

}