/*
 * Lora to Lora (P2P) using RN2483 and ESP8266
 * This sketch will transmit a lora message with payload "20" every 2 seconds. 
 * An LED will blink every time a package is sent. Payload and package ID is printed on serial monitor. 
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
SoftwareSerial loraSerial(6, 5); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);

String str;
int packageID = 0;

void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266
  led_off();
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600);  // Serial communication to PC
  Serial.println("Setup Begin");

  loraSerial.begin(57600);  // Serial communication to RN2483
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
  
//  loraSerial.println("radio set bt 0.5");  // Uncomment if we want to use FSK 
//  wait_for_ok();
  
  loraSerial.println("radio set mod lora"); // Comment if we want to use FSK 
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set freq 869100000");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set pwr 14");  //max power 14 dBm
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sf sf7");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set afcbw 41.7");
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set rxbw 125");
  str = loraSerial.readStringUntil('\n');
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
  
  loraSerial.println("radio set cr 4/5");
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

  Serial.println("starting loop");
}

void loop() {
  led_on();

  while (Serial.available() > 0) {
  str = Serial.readStringUntil("\n");
  str.trim();
  Serial.print("packageID = ");
  Serial.println(packageID);
  Serial.println("Message = " + str);
  loraSerial.print("radio tx ");
  //loraSerial.println("20");
  loraSerial.println(str);
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  led_off();
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

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
