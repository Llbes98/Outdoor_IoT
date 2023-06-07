/*
 * 
 * All the resources for this project: https://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
 
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

String teams[] = {"23 72 6C 17","13 69 FB 12","C3 8D 5B 11","43 FE D8 04"};
int team;
int greenLed = 6;
int redLed = 7;
long checkIn;

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();


}
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  checkIn = millis();
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  for (int i = 0; i < 4; i++)
  {
    if (content.substring(1) == teams[i]){
      team = i + 1;
      break;
    }
  }

  Serial.print("Team ");
  Serial.print(team);
  Serial.println(" Checked in");

  while (0 < team && team < 5 && checkIn > millis() - 30000) {
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, HIGH);
}