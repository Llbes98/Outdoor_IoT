/*
 * Lora to Lora (P2P) using RN2484 and ESP8266
 * This sketch will listen for any lora messages sent with correct SP, frequency, and other settings (see code below). 
 * An LED will blink every time a package is received. The received payload is printed on serial monitor. 
 * 
 * Code based on JP Meijers code from 30 September 2016
 * MN Petersen, Aug 2020
 * 
 * CONNECTIONS:
 * RN2483 - ESP8266:
 * TX     - D5
 * RX     - D6
 * 3V3    - 3V3
 * GND    - GND
 * 
 * D7 on ESP8266 is connected to anode (long leg) on LED. Insert resistor between cathode and GND on ESP8266.
 * 
 */
 
#include <SoftwareSerial.h>
SoftwareSerial loraSerial(14, 12); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);

String str;

void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266
  led_off();
  
  // Open serial communications and wait for port to open:
  
  Serial.begin(57600);
  
  loraSerial.begin(9600);
  loraSerial.setTimeout(1000);
  lora_autobaud();
  
  led_on();
  delay(1000);
  led_off();

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
  
  loraSerial.println("radio set wdt 60000"); //disable for continuous reception
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sync 12");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set bw 125");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
}

void loop() {
  Serial.println("waiting for a message");
  loraSerial.println("radio rx 0"); //wait for 60 seconds to receive
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  delay(5);
  if ( str.indexOf("ok") == 0 )
  {
    str = String("");
    while(str=="")
    {
      str = loraSerial.readStringUntil('\n');
    }
    if ( str.indexOf("radio_rx") == 0 )  //checking if data was received (equals radio_rx = <data>). indexOf returns position of "radio_rx"
    {
      if(str == "47617465576179"){
        loraSerial.print("radio tx ");
        loraSerial.println("506f6431");
        str = loraSerial.readStringUntil('\n');
        Serial.println(str);
        str = loraSerial.readStringUntil('\n');
        Serial.println(str);
      }
      Serial.println(str); //printing received data
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

void toggle_led()
{
  digitalWrite(13, 1);
  delay(100);
  digitalWrite(13, 0);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
