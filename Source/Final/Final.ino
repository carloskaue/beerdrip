#include "Timers.h"
#include "MainMachine.h"
#include "Spiff.h"

MainMachine mainMachine(1);

void setup()
{
	Serial.begin(115200);
	while (!Serial) continue;

	mainMachine.begin(1);
}

void loop()
{
	mainMachine.mainLoop();
	
	delay(1000);

}