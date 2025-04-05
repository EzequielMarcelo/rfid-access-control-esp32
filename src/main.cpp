//---- Auxiliary Libraries ----
#include <Arduino.h>
#include <SPI.h>                                      
#include <MFRC522.h>                                  

//---- Hardware Mapping ----
#define   RFID_SS_PIN    21 
#define   RFID_RST_PIN   22  
#define   LED            2

//---- Global Variables ----
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); 

// ---- Scope of Fuctions ----
void rfid_read();                          

void setup() 
{
  Serial.begin(9600);
  SPI.begin();          
  mfrc522.PCD_Init(); 

  pinMode(LED, OUTPUT);
  
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() 
{
  rfid_read();  
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