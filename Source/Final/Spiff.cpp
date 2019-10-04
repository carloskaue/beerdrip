#include "Spiff.h"

Spiff::Spiff(String str)
{
	PATH = str;
}

bool Spiff::create()
{
	if(SPIFFS.exists(PATH))	
	{
		Serial.println("Arquivo existe");
		return true;
	}
	else
	{
		Serial.println("Arquivo não existe");
		file = SPIFFS.open(PATH, "w+");

		if(file)
		{
			Serial.println("Arquivo criado");
			file.close();
			return true;
		}
	}
	Serial.println("Erro ao criar o arquivo");
	file.close();
	return false;
}

bool Spiff::begin()
{
	//Abre o sistema de arquivos
	if(SPIFFS.begin())
	{
		Serial.println("\nSistema de arquivos aberto com sucesso!");
		 // verifica se o arquivo existe		
		if(create()) 
		{
			return true;
		}
		
	}

	// Serial.println("\nErro ao abrir o sistema de arquivos\n\n");
	// ESP.restart();
	// return false;
}

bool Spiff::Writeline(String str)
{
	if(SPIFFS.exists(PATH))	
	{
		file = SPIFFS.open(PATH,"w");
		if(file)
		{
			file.println(str);
			file.close();
			return true;
		}
		else
		{
			file.close();
			return false;
		}

	}
}

String Spiff::ReadLine(uint8_t offset)
{
	if(SPIFFS.exists(PATH))	
	{
		String str;
		file = SPIFFS.open(PATH,"r");
		//if(offset == 0) offset = 1;
		if(offset < file.size())
		{
			for(int i = 0; i<=offset; i++)
			{
				str = file.readStringUntil('\n');
			}
			Serial.println(str);
			return str;
		}
	}
	else return "";
}