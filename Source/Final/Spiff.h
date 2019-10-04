#ifndef SPIFF_H
#define SPIFF_H

#include <Arduino.h>
#include <FS.h>

class Spiff
{
public:
	Spiff(String path);
	bool begin();
	bool create();
	String ReadLine(uint8_t offset);
	bool Writeline(String str);
private:
	String PATH;
	File file;
	unsigned long position;
};
#endif
