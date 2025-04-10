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
#define DELAY_BETWEEN_READS 1000  // Milliseconds

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
void add_card(const char *address);
bool card_is_registred(const char *address);   
bool card_is_adm(const char *address); 
void remove_card(const char *address);                

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
  
  Serial.println(currentSettings.card_amount);
  Serial.println(currentSettings.cardAddress[0]);
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() 
{
  static int currentState = RFID_READ;
  char address[12];
  static unsigned long timeLastRead = 0;

  if(!digitalRead(MASTER_PIN))
    currentState = RFID_ADD_MASTER;
  
  if((millis() - timeLastRead) >= DELAY_BETWEEN_READS)
  {
    switch (currentState)
    {
      case RFID_READ:
        if(rfid_read(address))
        {
          Serial.println(currentSettings.masterCardAddress);
          if(card_is_adm(address))
          {
            Serial.println("Modo cadastro");
            currentState = RFID_ADD_OR_REMOVE_SLAVE;
          }
          else if(card_is_registred(address))
          {
            open_door();
          }
          else
          {
            Serial.println("Cartão não registrado");
          }
        }        
        break;

      case RFID_ADD_OR_REMOVE_SLAVE:
        if(rfid_read(address))
        {
          if(card_is_adm(address))
          {
            currentState = RFID_READ;
            return;
          }
          if(!card_is_registred(address))
            add_card(address);

          else
            remove_card(address);
        }
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
      timeLastRead = millis();
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

void add_card(const char *address)
{
  Serial.print("Adicionando cartão: ");
  Serial.println(address);

  strncpy(currentSettings.cardAddress[currentSettings.card_amount], address, sizeof(currentSettings.cardAddress[currentSettings.card_amount]) - 1);
  currentSettings.cardAddress[currentSettings.card_amount][sizeof(currentSettings.cardAddress[currentSettings.card_amount]) - 1] = '\0';

  currentSettings.card_amount++;
  Settings.Save(currentSettings);

  if(currentSettings.card_amount == MAX_CARDS)
    currentSettings.card_amount = 0;
}

bool card_is_registred(const char *address)
{
  bool success = false;

  for(uint8_t current_address = 0; current_address < currentSettings.card_amount; current_address++)
  {
    if(!strcmp(currentSettings.cardAddress[current_address], address))
    {
      success = true;
      break;
    }
  }
  return success;
}

bool card_is_adm(const char *address)
{
  if(!strcmp(currentSettings.masterCardAddress, address))
    return true;
  
  return false;
}

void remove_card(const char *address)
{
  bool success = false;

  Serial.print("Removendo cartão: ");
  Serial.println(address);

  for(uint8_t card_remove = 0; card_remove < currentSettings.card_amount; card_remove++)
  {
    if(!strcmp(currentSettings.cardAddress[card_remove], address))
    {
      success = true;

      for(uint8_t card = card_remove; card < currentSettings.card_amount - 1; card++)
      {
        strncpy(currentSettings.cardAddress[card], currentSettings.cardAddress[card + 1], sizeof(currentSettings.cardAddress[card]) - 1);
        currentSettings.cardAddress[card][sizeof(currentSettings.cardAddress[card]) - 1] = '\0';
      }

      currentSettings.card_amount--;
      Settings.Save(currentSettings);

      Serial.println("Cartão removido com sucesso.");
      break;
    }
  }

  if(!success)
    Serial.println("Cartão não encontrado.");
}