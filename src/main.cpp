//---- Auxiliary Libraries ----
#include <Arduino.h>
#include <SPI.h>                                      
#include <MFRC522.h>       
#include "BLEClientLibrary.h"                           

//---- Hardware Mapping ----
#define   RFID_SS_PIN    21 
#define   RFID_RST_PIN   22  
#define   LED            2

//---- Auxiliary Constants ----
#define ADDRESS          "fc:58:fa:b3:88:21"

//---- Global Variables ----
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); 
BLEClientLibrary BLE;

// ---- Scope of Fuctions ----
void rfid_read(); 
void button_click();                         

void setup() 
{
  Serial.begin(9600);
  SPI.begin();          
  mfrc522.PCD_Init(); 
  BLE.begin();
  BLE.SetNotificationCallBack(&button_click);

  pinMode(LED, OUTPUT);
  
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() 
{
  rfid_read(); 
  BLE.StartScan();  
  if(BLE.GetFoundDevice())
  {
    Serial.println("Connecting...");

    const char *address = BLE.GetFoundDevice()->getAddress().toString().c_str();

    if(!strcmp(ADDRESS, address))
    {
      BLE.Connect(BLE.GetFoundDevice());
    }  
  } 
}

// ---- Auxiliary Functions ----
void rfid_read()                            
{
  
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
      
  Serial.print("Tag UID: ");
  String conteudo= "";
  for(byte i = 0; i < mfrc522.uid.size; i++) 
  {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  if(conteudo.substring(1) == "86 D0 17 7E")
  {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);         
  }
} 

void button_click()
{
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);  
}