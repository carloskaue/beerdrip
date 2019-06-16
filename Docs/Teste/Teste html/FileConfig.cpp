#include "FileConfig.h"
#include <FS.h>

FileConfig::FileConfig(String path)
{   
   AddressFile = path;
}

String FileConfig::ToString()
{
  String string;
  File rFile = SPIFFS.open(AddressFile,"r");
  if (!rFile) {
   
   return "";
  }
  //string  = rFile.readStringUntil('\r'); //desconsidera '\r\n'
  string  = rFile.readString();
  rFile.close();
  return string;
}

String FileConfig::begin()
{
	String string = "";
	if(!SPIFFS.begin())
  	{
    	return ("");
  	} 

	// verifica se o arquivo existe
  	if(SPIFFS.exists(AddressFile))
  	{
    	// O arquivo existe
    	////////////////////

  		string += "O arquivo existe\n";
    	// Abre como leitura
    	file = SPIFFS.open(AddressFile, "r");


   		if (!file) 
    	{
      		// Não abriu o arquivo
      		//////////////////////

      		// sinaliza que não abriu o arquivo
      		string += "não abriu o arquivo\n";
      		return string;
    	}
    	file.close();
    }
    else
	{
	    // O arquivo não existe 
	    ///////////////////////
		 string += "O arquivo não existe\n";
	    // Abre como escrita
	    file = SPIFFS.open(AddressFile, "w");

	    // Verifica se ocorreu a abertura do arquivo
	    if (!file) 
	    {
	      // Não abriu o arquivo
	      //////////////////////

	      // sinaliza que não abriu o arquivo
	      string += "não abriu o arquivo\n";
	    }

    	file.close();
	}
    	

	string += "Sucesso em abrir o arquivo";
	return string;
}

void FileConfig::Save(String string)
{
  // Abre como escrita
  file = SPIFFS.open(AddressFile, "w+");
  string.replace  ("\r","");
  file.println(string);
  file.close();
}