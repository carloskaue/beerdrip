
#include "MainMachine.h"

MainMachine mainMachine(1);

void setup()
{
	Serial.begin(115200);
	while (!Serial) continue;

	mainMachine.begin(1);
	pinMode(SS, OUTPUT);
}

void loop()
{
	mainMachine.mainLoop();
}