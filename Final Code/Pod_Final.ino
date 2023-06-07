/*
  This code is made for the bachelor project "IoT for outdoor learning" by Lucas Lydik Bessing and Magnus Lydik Bessing
  This code is for pods, specifically pod 01
  When the pod checks if data is equal to 3031, this is specific for pod 01. For other pods it would be 3031, 3033 etc.
  Communication with gateway happens via LoRa 868 MHz
  Fritzing diagram can be found in bachelor thesis
*/
 
// required libraries
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID Variables
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String teams[] = {"23 72 6C 17","13 69 FB 12","C3 8D 5B 11","43 FE D8 04"};
int team;
long checkIn;
long teamCheckIn[] = {0,0,0,0};

// softwareserial for UART communication with RN2483 LoRa module
SoftwareSerial loraSerial(6, 5); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);
String str;

// other variables
String lastMessage;
bool received;
int switch1 = 2;
int switch2 = 3;
int switch3 = 4;
int switch4 = 7;
int greenLed = A0;
int redLed = A1;
int blueLed = A2;
int buzzer = A3;
int buttonPressed = 0;
int lastButtonPressed;
int answer;
int mode = 1;
long lastUpdate = 60000 * 2;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Initialize pins as input or outputs
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.println("Setup Begin");

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  loraSerial.begin(57600);
  loraSerial.setTimeout(1000);
  lora_autobaud();

  Serial.println("Initing LoRa");
  
  loraSerial.listen();
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  loraSerial.println("sys get ver");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("mac pause");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
//  loraSerial.println("radio set bt 0.5");
//  wait_for_ok();
  
  loraSerial.println("radio set mod lora");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set freq 869100000");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set pwr 14");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sf sf12");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set afcbw 41.7");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set rxbw 20.8");  // Receiver bandwidth can be adjusted here. Lower BW equals better link budget / SNR (less noise). 
  str = loraSerial.readStringUntil('\n');   // However, the system becomes more sensitive to frequency drift (due to temp) and PPM crystal inaccuracy. 
  Serial.println(str);
  
//  loraSerial.println("radio set bitrate 50000");
//  wait_for_ok();
  
//  loraSerial.println("radio set fdev 25000");
//  wait_for_ok();
  
  loraSerial.println("radio set prlen 8");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set crc on");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set iqi off");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set cr 4/5"); // Maximum reliability is 4/8 ~ overhead ratio of 2.0
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set wdt 10000"); //disable for continuous reception
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sync 12");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set bw 125");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  // When setup is done CorrectSound is played
  CorrectSound();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  // Core of Loop. Will run differently depending on mode variable
  // mode variable will always be equal to 1 or 2
  // In mode 1 pods will wait for a message from gateway containing its ID
  // When message is received confirmation message will be transmitted
  if (mode == 1) {
    // Entering receive mode waiting more question
    Serial.println("waiting for a message");
    loraSerial.println("radio rx 0"); //wait for 10 seconds to receive
    str = loraSerial.readStringUntil('\n');
    Serial.println(str);
    delay(20);
    if ( str.indexOf("ok") == 0 )
    {
      str = String("");
      while(str=="")
      {
        str = loraSerial.readStringUntil('\n');
      }
      if ( str.indexOf("radio_rx") == 0 )  //checking if data was received (equals radio_rx = <data>). indexOf returns position of "radio_rx"
      {
        // if message is received with correct pod ID
        if (str.substring(10,14) == "3031") { 
          // if it is a new message it is saved and received variable is set to false.
          // answer variable is also set accordingly
          if ((str != lastMessage)) {
            Serial.println("if 1");
            lastMessage = str;
            answer = str.substring(14,16).toInt() - 30;
            Serial.print("Answer = ");
            Serial.println(answer);

            // Send confirmation after short delay
            delay(100);
            loraSerial.print("radio tx ");
            loraSerial.println("3031");
            str = loraSerial.readStringUntil('\n');
            Serial.println(str);
            str = loraSerial.readStringUntil('\n');
            Serial.println(str);
            mode = 2;
          }
        }
      }
      else
      {
        Serial.println("Received nothing");
      }
    }
    else
    {
      Serial.println("radio not going into receive mode");
      delay(1000);
    }
  }
  else if(mode == 2){ // ------------------------------------------------------------ Mode 2
    // checks for update messages every 5 minutes
    // when this runs the LED will light purple
    if (lastUpdate < millis() - (60000 * 5)) {
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);
      digitalWrite(blueLed, HIGH);
      lastUpdate = millis();
      Serial.println("Checking for updates");
      loraSerial.println("radio rx 0"); //wait for 60 seconds to receive
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);
      delay(20);
      if ( str.indexOf("ok") == 0 )
      {
        str = String("");
        while(str=="")
        {
          str = loraSerial.readStringUntil('\n');
        }
        if ( str.indexOf("radio_rx") == 0 )  //checking if data was received (equals radio_rx = <data>). indexOf returns position of "radio_rx"
        {
          // if message is received and it is an end message shift to mode 1
          if (str.substring(10,12) == "45") {
            Serial.println(str);
            mode = 1;
          }
        }
        else
        {
          Serial.println("Received nothing");
        }
      }
      else
      {
        Serial.println("radio not going into receive mode");
        delay(1000);
      }
      digitalWrite(redLed, LOW);
      digitalWrite(blueLed, LOW);
    }  

    // Look for new RFID cards, if no cards are found return
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the RFID cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }

    // Saves time when a card is read
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
    
    content.toUpperCase();

    for (int i = 0; i < 4; i++)
    {
      // checks if UID of RFID card corresponds to a team UID
      if (content.substring(1) == teams[i]){
        // If match is found a check is made to find out wether that team has been checked in less than 30 seconds ago
        if (teamCheckIn[i] < millis() - 90000){
          // If not the team variable is set to the corresponding team and a check in time is stored
          team = i + 1;
          teamCheckIn[i] = millis();
          break;
        }
        else {
          // If the team checked in less than 30 seconds ago the LED flashes red and doesn't set the team variable
          digitalWrite(redLed, HIGH);
          WrongSound();
          delay(1000);
          digitalWrite(redLed, LOW);
          break;
        }
      }
      else {
        // If UID doesn't match to a team the team variable is set to 0
        team = 0;
      }
    }

    // Serial prints for troubleshooting
    Serial.print("Team ");
    Serial.print(team);
    Serial.println(" Checked in");

    // If a team has checked in the team variable will be between 1 and 4
    // loop will run as long as the team hasn't pressed a button or 30 seconds has passed
    while (0 < team && team < 5 && checkIn > millis() - 30000) {
      // checks for buttons pressed
      if (!digitalRead(switch1)) {
        buttonPressed = 1;
      }
      else if (!digitalRead(switch2)) {
        buttonPressed = 2;
      }
      else if (!digitalRead(switch3)) {
        buttonPressed = 3;
      }
      else if (!digitalRead(switch4)) {
        buttonPressed = 4;
      }
      else {
        buttonPressed = 0;
      }

        // if correct button is pressed LED will light green and a correctSound is played
        // PodID and team number is also transmitted to gateway till new question is received
        if (buttonPressed == answer) {
          digitalWrite(greenLed, HIGH);
          digitalWrite(redLed, LOW);
          digitalWrite(blueLed, LOW);
          CorrectSound();

          received = false;
          // when new question is not received this loop will run
          while (!received) {
            // transmits pod ID and team number
            Serial.println("Correct Answer");
            loraSerial.print("radio tx ");
            loraSerial.print("3031");
            loraSerial.println(team + 30);
            str = loraSerial.readStringUntil('\n');
            Serial.println(str);
            str = loraSerial.readStringUntil('\n');
            Serial.println(str);
            
            // waits for new question
            Serial.println("waiting for a message");
            loraSerial.println("radio rx 0"); //wait for 10 seconds to receive
            str = loraSerial.readStringUntil('\n');
            Serial.println(str);
            delay(20);
            if ( str.indexOf("ok") == 0 )
            {
              str = String("");
              while(str=="")
              {
                str = loraSerial.readStringUntil('\n');
              }
              if ( str.indexOf("radio_rx") == 0 )  //checking if data was received (equals radio_rx = <data>). indexOf returns position of "radio_rx"
              {
                // if message is received with correct pod ID Received variable is set to true
                if (str.substring(10,14) == "3031") {
                  Serial.println(str);
                  received = true;
                  // if not a confirmation message answer is set accordingly
                  if (str.substring(14,16) != "43") {
                    answer = str.substring(14,16).toInt() - 30;
                    Serial.print("Answer = ");
                    Serial.println(answer);
                    
                    // send confirmation after short delay
                    delay(100);
                    loraSerial.print("radio tx ");
                    loraSerial.println("303143");
                    str = loraSerial.readStringUntil('\n');
                    Serial.println(str);
                    str = loraSerial.readStringUntil('\n');
                    Serial.println(str);
                  }
                }
              }
              else
              {
                Serial.println("Received nothing");
              }
            }
            else
            {
              Serial.println("radio not going into receive mode");
              delay(1000);
            }
          }
          // team is reset after question is received
          team = 0;
        }
        // if no button is pressed the LED lights blue
        else if (buttonPressed == 0) {
          digitalWrite(greenLed, LOW);
          digitalWrite(redLed, LOW);
          digitalWrite(blueLed, HIGH);
        }
        // if a button is pressed and it is not the correct one LED will light Red, WrongSound will play and team will be resat
        else {
          Serial.print("Wrong Answer!");
          digitalWrite(greenLed, LOW);
          digitalWrite(redLed, HIGH);
          digitalWrite(blueLed, LOW);
          WrongSound();
          delay(1000);
          team = 0;
        }
    }
    // when loop is over LED wil turn off
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
  }
}

// -----------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------Functions
// -----------------------------------------------------------------------------------------------------

void lora_autobaud()
{
  String response = "";
  while (response=="")
  {
    delay(1000);
    loraSerial.write((byte)0x00);
    loraSerial.write(0x55);
    loraSerial.println();
    loraSerial.println("sys get ver");
    response = loraSerial.readStringUntil('\n');
  }
}

/*
 * This function blocks until the word "ok\n" is received on the UART,
 * or until a timeout of 3*5 seconds.
 */
int wait_for_ok()
{
  str = loraSerial.readStringUntil('\n');
  if ( str.indexOf("ok") == 0 ) {
    return 1;
  }
  else return 0;
}

// sound effects for right answer
void CorrectSound() {
  tone(buzzer,440*5);
  delay(100);
  tone(buzzer,554*5);
  delay(100);
  tone(buzzer,440*5);
  delay(100);
  tone(buzzer,554*5);
  delay(350);
  noTone(buzzer);
}

// sound effects for wrong answer
void WrongSound() {
  tone(buzzer,784);
  delay(250);
  tone(buzzer,523);
  delay(500);
  noTone(buzzer);
}

