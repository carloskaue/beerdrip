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

void MspComunication::begin(uint16_t bits)
{
	SPI.begin();
	SPI.beginTransaction(SPISettings(wFrequency, MSBFIRST, SPI_MODE1));
	setDataBits(bits);

	// Aquivo dos dados
	fileBeer.begin();
}
uint8_t MspComunication::transfer(uint8_t data)
{
	//SPI.transfer(data);
	while(SPI1CMD & SPIBUSY) {}
	SPI1W0 = data;
	SPI1CMD |= SPIBUSY;
	while(SPI1CMD & SPIBUSY) {}
	return (uint8_t)(SPI1W0&0xFF);
}

//TODO: método para verificar a mensagem recebida bate com a esperada.
bool MspComunication::getMSPStart(void)
{
	String str = "";
	unsigned char data[2]= {0,0};
	unsigned char indice = 0;
	unsigned char cont = 0;
	 
	for (uint8_t indice = 0; indice < 3; indice ++)
	{
		transfer(FrameStartig[indice]);
	}
	while (cont < 10)
	{
		data[indice]=transfer(0);
		if(data[indice] == 0x02)
		{
			indice++;
		}
		else if(data[indice] == 0xE0)
		{
			indice =0;
		}
		else if(data[indice] == 0x03)
		{
			return true;
		}
		cont++
	}
	return false;
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
bool MspComunication::setDataBits(uint16_t bits) {
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}