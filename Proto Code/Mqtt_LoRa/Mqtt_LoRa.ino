#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial loraSerial(14, 12); // (rxPin (D5), txPin (D6));
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

// Timers auxiliar variables
long lastPub = 0;

// Mode variable
int mode = 0;
int q = 0;
String Question;
String lastQuestion;

bool podsReceived[] = {0,0};

// This functions connects your ESP8266 to your router
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
  char hexBuffer[2 * messageTemp.length() + 1];  // Create a buffer large enough to hold the hex string
  Serial.println(messageTemp);
  stringToHex(messageTemp, hexBuffer);
  Serial.println(hexBuffer);

  if(topic == "GameMode") {
    mode = messageTemp.toInt();
  }
  else if(topic == "Questions") {
    Question = hexBuffer;
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
      client.subscribe("Questions");
      client.subscribe("GameMode");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 15593);
  client.setCallback(callback);

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

}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()) {
    client.connect("GateWay", MQTT_username, MQTT_password);
  }

  if(mode == 1) {
    if (Question != lastQuestion) {
      setArrayToFalse(podsReceived, 2);
      lastQuestion = Question;
    }

    if(!allTrue(podsReceived, 2)){
      // receive confirmation from pods
      Serial.println("not all pods confirmed");
      Serial.print("Question Send = ");
      Serial.println(Question);
      loraSerial.print("radio tx ");
      loraSerial.println(Question);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);

      Serial.println("waiting for confirmation");
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
          int podNr = str.substring(11).toInt();
          Serial.print("Pod ");
          Serial.print(podNr);
          Serial.println(" received message");
          podsReceived[(podNr-1)] = true;

          Serial.print("Send Confirmation to");
          Serial.println(podNr);
          loraSerial.print("radio tx ");
          loraSerial.print("43");
          loraSerial.println(podNr);
          str = loraSerial.readStringUntil('\n');
          Serial.println(str);
          str = loraSerial.readStringUntil('\n');
          Serial.println(str);
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

      if (allTrue(podsReceived, 2)){
        q++;
        Serial.print("requesting new question: Nr. ");
        Serial.println(q);
        client.publish("Answers", String(q).c_str());
      }
    }
  }
  else if (mode == 2) {
    
  }
  else {
    
  }

} 

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

void stringToHex(const String& inputString, char* hexBuffer) {
  int strLength = inputString.length();
  
  for (int i = 0; i < strLength; i++) {
    sprintf(hexBuffer + (i * 2), "%02X", inputString[i]);
  }
  
  hexBuffer[strLength * 2] = '\0';  // Null-terminate the hex string
}

bool allTrue(const bool arr[], int size) {
  for (int i = 0; i < size; i++) {
    if (!arr[i]) {
      return false;
    }
  }
  return true;
}

void setArrayToFalse(bool arr[], int size) {
  for (int i = 0; i < size; i++) {
    arr[i] = false;
  }
}