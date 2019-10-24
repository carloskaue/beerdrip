#include <Timers.h>

Timers::Timers(unsigned long bytTimer)
{
	timeout = bytTimer;   
	mulTimeCurrent = 0;
	mulTimeLast = millis();
	mwTimer = 0;
}

// Define o tempo para timeout
void Timers::settimer(uint16_t wTimer)
{
   mwTimer = wTimer;
   Serial.print("timeout definido em ");
   Serial.println(mwTimer);
}

// Verifica se há timeout
bool Timers::IsTimeout()
{
	 mulTimeCurrent = millis();

	//Verifica se passou o tempo para o timeout
	/////////////////////////////////////////////

	// verifica se houve um overflow
	if(mulTimeCurrent > mulTimeLast)
	{
		// Não houve um overflow
		/////////////////////////
		
		// Verifica se ocorreu um timeout
		if(mulTimeCurrent - mulTimeLast >= timeout)
		{
			// Ocorreu um timeout para decrementar o timer
			//////////////////////////////////////////////

			mulTimeLast = mulTimeCurrent;

			// Verifica se o timer está rotando
			if(mwTimer)
			{
				if(--mwTimer==0)
				{
					// Houve um timeout real
					return true;
				}
			}
		}
	}
	// Não Houve um timeout real.
	return false;
}

// Salva o tempo inicial para referencia
void Timers::GetInitialTime(unsigned long timer)
{
	mulItialTimer = timer;
}

// Salva o tempo para utilizar na comparação entre o tempo do servidor e a resposta do msp
void Timers::GetTheTimeFromTheStart()
{
	mulTimeFromTheStart = millis();
}

// Define o tempo inical dos dados
void Timers::SetInitializationTimerFromData()
{
	// Pega o tempo entre a atualização no servidor e a resposta do msp em millisegundos.
	unsigned long TimeInMillis = millis() - mulTimeFromTheStart;

	// Verifica se houve mais que meio segundo para o msp
	// responder depois que da atualização do servido NTP
	if( TimeInMillis  > 500)
	{
		// Houve mais que meio segundo até a resposta do msp
		////////////////////////////////////////////////////

		// Define a hora( NTP )com a correção do tempo para a resposta do msp.
		mulTimerFromData = mulItialTimer + (TimeInMillis + 500)/1000;

	}}

unsigned long Timers::GetTimerFromData()
{
	return mulTimerFromData;
}
