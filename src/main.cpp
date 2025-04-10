//---- Auxiliary Libraries ----
#include <Arduino.h>
#include <SPI.h>                                      
#include <MFRC522.h>       
#include "BLEClientLibrary.h"    
#include "SettingsManager.h"                       

//---- Hardware Mapping ----
#define   RFID_SS_PIN    21 
#define   RFID_RST_PIN   22  
#define   LED            2
#define   LOCK           4
#define   MASTER_PIN     5   

//---- Auxiliary Constants ----
#define ADDRESS          "fc:58:fa:08:96:b5"
#define DELAY_BETWEEN_OPEN  5000  // Milliseconds

enum RFID_STATE
{
  RFID_READ = 0,
  RFID_ADD_OR_REMOVE_SLAVE,
  RFID_ADD_MASTER
};

//---- Global Variables ----
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); 
BLEClientLibrary BLE;
SettingsManager Settings;

settings_t currentSettings;

// ---- Scope of Fuctions ----
bool rfid_read(char *address); 
void button_click();  
void open_door();   
void load_default_settings();                     

void setup() 
{
  Serial.begin(9600);
  SPI.begin();          
  mfrc522.PCD_Init(); 
  BLE.begin();
  BLE.SetNotificationCallBack(&button_click);
  Settings.begin();

  pinMode(LED, OUTPUT);
  pinMode(LOCK, OUTPUT);
  pinMode(MASTER_PIN, INPUT_PULLUP);
  digitalWrite(LOCK, HIGH);

  if(!Settings.Load(&currentSettings))
    load_default_settings();
  
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() 
{
  static int currentState = RFID_READ;
  char address[20];

  if(!digitalRead(MASTER_PIN))
    currentState = RFID_ADD_MASTER;
  
  switch (currentState)
  {
    case RFID_READ:
      if(rfid_read(address))
        {
          Serial.println(currentSettings.masterCardAddress);
          if(!strcmp(currentSettings.masterCardAddress, address))
          {
            Serial.println("Modo cadastro");
            currentState = RFID_ADD_OR_REMOVE_SLAVE;
          }
        }
      break;

    case RFID_ADD_OR_REMOVE_SLAVE:
      /* code */
      break;
    case RFID_ADD_MASTER:
      if(rfid_read(address))
      {
        strcpy(currentSettings.masterCardAddress, address);
        Settings.Save(currentSettings);
        Serial.print("Nova tag ADM: ");
        Serial.println(currentSettings.masterCardAddress);
        currentState = RFID_READ;
      }
      break;
  
    default:
      currentState = RFID_READ;
      break;
  }
}

// ---- Auxiliary Functions ----
bool rfid_read(char *address)
{  
  if (!mfrc522.PICC_IsNewCardPresent())
    return false;

  if (!mfrc522.PICC_ReadCardSerial()) 
    return false;

  Serial.print("Tag UID: ");
  String conteudo = "";
  for(byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    
    conteudo.concat(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();

  conteudo.toUpperCase(); 
  conteudo.toCharArray(address, conteudo.length() + 1);

  return true;
}

void button_click()
{
  open_door();
}

void open_door()
{
  static unsigned long timeLastOpened = 0;

  if((millis() - timeLastOpened) >= DELAY_BETWEEN_OPEN)
  {
    digitalWrite(LED, HIGH);
    digitalWrite(LOCK, LOW);
    delay(100);
    digitalWrite(LED, LOW);  
    digitalWrite(LOCK, HIGH); 
    timeLastOpened = millis();
  }
}
void load_default_settings()
{
  memset(&currentSettings, 0, sizeof(settings_t));
}