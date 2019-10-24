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
	bool Write(String str);
	void ResetPosition(void);
	void SetNewPosition(void);
	String  ReadLineToSend();
	bool AlreadyReachedEndOfFile(void);
	void RemoveFile(void);
	
private:
	String PATH;
	File file;
	unsigned long mlNextposition;
	unsigned long mlOldposition;
};
#endif
