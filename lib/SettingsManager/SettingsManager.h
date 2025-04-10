#ifndef SettingsManager_H_
#define SettingsManager_H_

#include "SPIFFS.h"
#include "FS.h"

#define SETTINGS_PATH "/settings.dat"
#define MAX_CARDS 200

typedef struct 
{
    char masterCardAddress[20];
    char cardAddress[MAX_CARDS][20];
    uint8_t card_amount;
}settings_t;

class SettingsManager
{
    private:
        
    public:
        bool begin();
        bool Load(settings_t* settings);
        bool Save(settings_t settings); 
        bool Delete(); 
        void ListFiles(const char* dir);    
};

#endif