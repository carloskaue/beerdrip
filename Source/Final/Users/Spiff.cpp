#include <Spiff.h>

/*
 * "r"	Abre um arquivo texto para leitura. 
 *  	o arquivo deve existir antes de ser aberto.
 * "w"	Abrir um arquivo texto para gravação. 
 * 		Se o arquivo não existir, ele será criado. 
 * 		Se já existir, o conteúdo anterior será destruído. 
 * "a"	Abrir um arquivo texto para gravação. Os dados serão 
 * 		adicionados no fim do arquivo ("append"), se ele já 
 * 		existir, ou um novo arquivo será criado, no caso de 
 * 		arquivo não existente anteriormente.
 * "r+"	Abre um arquivo texto para leitura e gravação. 
 * 		O arquivo deve existir e pode ser modificado.
 * "w+"	Cria um arquivo texto para leitura e gravação. 
 * 		Se o arquivo existir, o conteúdo anterior será destruído. 
 * 		Se não existir, será criado.
 * "a+"	Abre um arquivo texto para gravação e leitura. Os dados 
 * 		serão adicionados no fim do arquivo se ele já existir, ou um 
 * 		novo arquivo será criado, no caso de arquivo não existente anteriormente.
*/




Spiff::Spiff(String str)
{
	PATH = str;
}

// Cria o arquivo
bool Spiff::create()
{
	// Verifica se o arquivo exite
	if(SPIFFS.exists(PATH))	
	{
		// Arquivo ja exite
		return true;
	}
	else
	{
		// Arquivo ainda não exite
		
		// Cria o arquivo
		file = SPIFFS.open(PATH, "w+");

		// Verifica se o arquivo foi criado
		if(file)
		{
			// Arquivo criado
			///////////////////
			
			// Fecha o arquivo 
			file.close();
			return true;
		}
	}
	// Erro ao criar o arquivo
	file.close();
	return false;
}

bool Spiff::begin()
{
	//Abre o sistema de arquivos
	if(SPIFFS.begin())
	{
		 // verifica se o arquivo existe, se não existir cria o arquivo
		if(create()) 
		{
			return true;
		}
		
	}

	// TODO: Resetar o esp ao não conseguir abrir o arquivo?
	// Erro ao abrir o sistema de arquivos
	// ESP.restart();
	// return false;
}

// Escreve uma string no arquivo 
bool Spiff::Writeline(String str)
{
	// Verifica se o arquivo existe
	if(SPIFFS.exists(PATH))	
	{
		// Arquivo existe
		//////////////////

		// Abre o arquivo para escrever
		file = SPIFFS.open(PATH,"a+");
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

// Escreve uma string no arquivo 
bool Spiff::Write(String str)
{
	// Verifica se o arquivo existe
	if(SPIFFS.exists(PATH))	
	{
		// Arquivo existe
		//////////////////

		// Abre o arquivo para escrever
		file = SPIFFS.open(PATH,"w+");
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


// Lê uma determinada linha do arquivo
String Spiff::ReadLine(uint8_t offset)
{
	if(SPIFFS.exists(PATH))	
	{
		String str;
		file = SPIFFS.open(PATH,"r");
		if(offset < file.size())
		{
			for(int i = 0; i<=offset; i++)
			{
				str = file.readStringUntil('\n');
			}
			Serial.println(str);
			file.close();
			return str;
		}
	}
	else 
	{
		file.close();
		return "";
	}
}


// Lê uma linha e retorna pronta para ser enviada
String Spiff::ReadLineToSend(void)
{
	char buffer[64];
	String torneira;
	String quantidade;
	String tempo;

	// Abre o arquivo
	file = SPIFFS.open(PATH,"r");

	if (file.available()) 
	{
		// DEfine a posição
		file.seek(mlOldposition, SeekSet);
		int l = 0;
		// Faz a leitura da torneira
		l = file.readBytesUntil(';', buffer, sizeof(buffer));
		if(l > 0)
		{
			buffer[l] = '\0';
			torneira  = buffer;
		}else return "";
		// Faz a leitura da quantidade 
		l = file.readBytesUntil(';', buffer, sizeof(buffer));
		if(l > 0)
		{
			buffer[l] = '\0';
			quantidade  = buffer;
		}else return "";
		// Faz a leitura da hora
		l = file.readBytesUntil('\n', buffer, sizeof(buffer));
		if(l > 0)
		{
			buffer[l] = '\0';
			tempo  = buffer;
		}
		else return "";
		// Salva o indice atual
		mlNextposition = file.position();
		

		return ("{\"consumptions\":[{\"resource\":"+torneira+",\"quantity\":"+quantidade+",\"time\":"+tempo+"}]}");
	}
	else return "";
}

// Define uma nova posição baseda na ultima leitura
void Spiff::SetNewPosition()
{
	mlOldposition = mlNextposition;
}

// Volta para o começo do arquivo.
void Spiff::ResetPosition()
{
	mlNextposition = 0;
	SetNewPosition();
}

// Retorna se já está no final do arquivo
bool Spiff::AlreadyReachedEndOfFile(void)
{
	// TODO: verificar se envia todo os dados
	return	(mlNextposition >= file.size()?true:false);
}

void Spiff::RemoveFile()
{
	SPIFFS.remove(PATH);
}