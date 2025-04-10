#include "SettingsManager.h"

bool SettingsManager::begin()
{  
  return SPIFFS.begin(true);
}
bool SettingsManager::Load(settings_t *settings)
{
    if(!SPIFFS.exists(SETTINGS_PATH))
    {
        Serial.println("File doesn't exist");
        return false;
    }

    File file = SPIFFS.open(SETTINGS_PATH);
    
    if (!file) 
    {
        Serial.println("Failed to open file");
        return false;
    }
    
    file.read((uint8_t*)settings->masterCardAddress, sizeof(settings->masterCardAddress));
    file.read((uint8_t*)&settings->card_amount, sizeof(settings->card_amount));

    for (uint8_t card = 0; card < settings->card_amount; card++) 
        file.read((uint8_t*)settings->cardAddress[card], sizeof(settings->cardAddress[card]));

    file.close();

    return true;
}
bool SettingsManager::Save(settings_t settings)
{
    File file = SPIFFS.open(SETTINGS_PATH, "w");

    if (!file) 
    {
        Serial.println("Failed to open file to write");
        return false;
    }
    
    file.write((uint8_t*)settings.masterCardAddress, sizeof(settings.masterCardAddress));
    file.write((uint8_t*)&settings.card_amount, sizeof(settings.card_amount));
        
    for (uint8_t card = 0; card < settings.card_amount; card++) 
        file.write((uint8_t*)settings.cardAddress[card], sizeof(settings.cardAddress[card]));
    
    file.close();

    Serial.println("Data written to file successfully");
    return true;
}
bool SettingsManager::Delete()
{
    if (SPIFFS.remove(SETTINGS_PATH)) 
        return true;
    
    return false;    
}
void SettingsManager::ListFiles(const char* dir) 
{
    Serial.println("Listing files:");

    File root = SPIFFS.open(dir);

    if (!root) 
    {
        Serial.println("Failed to open directory");
        return;
    }

    File file = root.openNextFile();
    if (!file)
        Serial.println("Empty directory");
    else 
    {
        while (file) 
        {
            Serial.print("File: ");
            Serial.println(file.name()); 
    
            Serial.print("Size: ");
            Serial.print(file.size());
            Serial.println(" bytes");

            file = root.openNextFile();
        }

        file.close();
    }
}