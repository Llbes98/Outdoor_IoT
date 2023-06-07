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
#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <SoftwareSerial.h>
SoftwareSerial loraSerial(14, 12); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);
String str;


// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "HUAWEI P30";
const char* password = "Lucas1234";

// MQTT broker credentials (set to NULL if not required)
const char* MQTT_username = "spvgksre"; 
const char* MQTT_password = "uQcV1ed4Rfai"; 

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "hairdresser.cloudmqtt.com";
//For example
//const char* mqtt_server = "192.168.1.106";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This function is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  } 
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client", MQTT_username, MQTT_password)) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266
  led_off();
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  setup_wifi();
  client.setServer(mqtt_server, 15593);
  client.setCallback(callback);
  
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
  
  loraSerial.println("radio set sf sf7");
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
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()) {
    client.connect("GateWay", MQTT_username, MQTT_password);
  }

  Serial.println("waiting for a message");
  loraSerial.println("radio rx 0"); // wait for 60 seconds to receive
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  delay(20);
  if (str.indexOf("ok") == 0) {
    str = String("");
    while (str == "") {
      str = loraSerial.readStringUntil('\n');
    }
    if (str.indexOf("radio_rx") == 0) {
      toggle_led();
      Serial.println(str); // printing received data
      
      loraSerial.println("radio get snr");
      str = loraSerial.readStringUntil('\n');
      Serial.print("SNR = ");
      Serial.println(str);
      client.publish("SNR", str.c_str());
      loraSerial.println("radio get rssi");
      str = loraSerial.readStringUntil('\n');
      Serial.print("RSSI = ");
      Serial.println(str);
      client.publish("RSSI", str.c_str());

    } else {
      Serial.println("Received nothing");
    }
  } else {
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
