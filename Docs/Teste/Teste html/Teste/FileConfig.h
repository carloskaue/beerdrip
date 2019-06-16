#ifndef WIFICONFIG_H
#define WIFICONFIG_H



#include <Arduino.h>
#include <FS.h>

struct Config
{
    char SSID[64];
    char PASSWORD[64];
    
};

class FileConfig
{
    public:
        FileConfig(String path);
        String ToString();
        String begin();
        void Save(String string);
    private:
    String AddressFile;
    File file;
};

#endif // WIFICONFIG_H
