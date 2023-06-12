/*
  This code is made for the bachelor project "IoT for outdoor learning" by Lucas Lydik Bessing and Magnus Lydik Bessing
  This code is for a gateway exchanging data between endpoints called pods and a backend on NodeRed
  Communication between gateway and endpoints use LoRa 868 MHz
  Communication between gateway and backend use MQTT
  Fritzing diagram can be found in bachelor thesis
*/

// required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// softwareserial for UART communication with RN2483 LoRa module
SoftwareSerial loraSerial(14, 12); // (rxPin (D5), txPin (D6));
String str;

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "HUAWEI P30";
const char* password = "Lucas1234";

// MQTT broker credentials (set to NULL if not required)
const char* MQTT_username = "spvgksre"; 
const char* MQTT_password = "uQcV1ed4Rfai"; 

// Change the variable to your MQTT broker IP address
const char* mqtt_server = "hairdresser.cloudmqtt.com";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long lastPub = 0;

// Mode variable
int mode = 0;
int q = 1;
String Question;
String lastQuestion;
String message;
long endReceived;

bool newQuestion = false;
bool Received = false;

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
  // Stores message received as a string variable
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  // If statements depending on topic message arrives on.
  // Variables updated here will be used in the loop.
  // If topic equals "Mode" mode is set to whatever the message is (Will be between 0 and 2)
  if(topic == "Mode") {
    mode = messageTemp.toInt();
    q = 1;
  }
  // If topic equals "Questions" the message is converted to Hex and stored in the Question variable.
  // newQuestion variable also turns true
  else if(topic == "Questions") {
    char hexBuffer[2 * messageTemp.length() + 1];  // Create a buffer large enough to hold the hex string
    Serial.println(messageTemp);
    stringToHex(messageTemp, hexBuffer);
    Serial.println(hexBuffer);

    Question = hexBuffer;
    newQuestion = true;
  }
  // if topic equals "EndGame" current time is stored in endReceived variable and mode is set to 3.
  else if(topic == "EndGame") {
    endReceived = millis();
    mode = 3;
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
    if (client.connect("ESP8266Client", MQTT_username, MQTT_password)) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("Questions");
      client.subscribe("Mode");
      client.subscribe("EndGame");
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
// The callback function is what receives messages and 
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 15593);
  client.setCallback(callback);

  loraSerial.begin(57600);
  loraSerial.setTimeout(1000);
  lora_autobaud();

  // Initialising LoRa via UART
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

  // If statements keep connection to MQTT broker
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()) {
    client.connect("GateWay", MQTT_username, MQTT_password);
  }


  // Core of Loop. Will run differently depending on mode variable.
  // If mode variable does not equal 1, 2 or 3 nothing will happen.
  // In mode 1 if a question is received it will be send via LoRa
  // When question has been confirmed a new question is requested via MQTT
  if(mode == 1) {
    // Checks for new question
    // If question is new it will be stored as lastQuestion. Received and newQuestion will both be set to false
    if (Question != lastQuestion) {
      Received = false;
      lastQuestion = Question;
      newQuestion = false;
    }

    // When message is not confirmed this if statement runs once every loop
    if(!Received){
      // Send question via LoRa
      Serial.print("Question Send = ");
      Serial.println(Question);
      loraSerial.print("radio tx ");
      loraSerial.println(Question);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);

      // Start receiving
      Serial.println("waiting for confirmation");
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
          // if message is confirmed Received is set to true
          int podNr = str.substring(10,14).toInt();
          Serial.print("Pod ");
          Serial.print(podNr);
          Serial.println(" received message");
          Received = true;
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

      // If message is confirmed q is incremented and published to "Init" Topic requesting new question
      if (Received){
        q++;
        Serial.print("newQuestion status is: ");
        Serial.println(newQuestion);
        Serial.print("requesting new question: Nr. ");
        Serial.println(q);
        client.publish("Init", String(q).c_str());
      }
    }
  }
  // In mode 2 the gateway will listen continuously for answers from pods.
  // when answer is received it is published to backend
  // when new question arrives it is send to pod
  else if (mode == 2) {
    // this runs whenever a message send is received by pods
    // since mode is changed when last pod has received its question this will be true when mode changes
    if(Received) {
      // waiting for message from pods
      Serial.println("waiting for messages");
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
          // If message is received, the Received variable turns false
          // Message is also saved in variable both as hex and ascii
          Received = false;
          message = str.substring(10);
          String asciiString;
          hexToAscii(message, asciiString);
          // if message is not a confirmation message it is published on "Answers" Topic
          if (message.substring(14,16) != "43") {
          int podNr = message.substring(0,4).toInt();
          Serial.println("Receieved message from pod: " + message.substring(0,4));
          Serial.println("Message: " + message);
          Serial.println("Message as ascii: " + asciiString);

          client.publish("Answers", asciiString.c_str());
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

    // if a confirmation message is not received yet AND a new question is received this will run
    // This only runs when a new question is received since it otherwise would run as soon as a message has been received by pods
    // in this case the wrong question would be send
    if (!Received && newQuestion) {
      // Sending new question via LoRa
      Serial.println("Sending: " + Question + " to: " + message.substring(0,4));
      loraSerial.println("radio tx " + Question);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);
      str = loraSerial.readStringUntil('\n');
      Serial.println(str);

      // Entering receive mode and waits for confirmation
      Serial.println("waiting for confirmation");
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
          // if received message is a confirmation message Received becomes true and newQuestion becomes false
          if (str.substring(14,16) == "43"){
            Received = true;
            newQuestion = false;
            Serial.println("new question confirmed");
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
  }
  // In mode 3 End message is continuously transmitted until 5 minutes from end message arrived from backend
  else if (mode == 3) {
    loraSerial.println("radio tx 45");
    str = loraSerial.readStringUntil('\n');
    Serial.println(str);
    str = loraSerial.readStringUntil('\n');
    Serial.println(str);

    if (endReceived < millis() - 6000*5) {
      mode = 0;
    }
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

// This function converts a given string to its hexadecimal representation and stores it in a character buffer.
void stringToHex(const String& inputString, char* hexBuffer) {
  int strLength = inputString.length();  // Get the length of the input string
  
  // Iterate over each character of the input string
  for (int i = 0; i < strLength; i++) {
    // Convert the current character to its hexadecimal representation and store it in the buffer
    // "%02X" specifies the format for converting the character to hexadecimal (two digits with leading zeros if necessary)
    // hexBuffer + (i * 2) calculates the address where the current hexadecimal representation should be stored
    sprintf(hexBuffer + (i * 2), "%02X", inputString[i]);   
  }
  // After converting all characters, add a null character ('\0') at the end of the buffer to terminate the string
  hexBuffer[strLength * 2] = '\0';  // Null-terminate the hex string
}

// This function converts a given hexadecimal string to its ASCII representation.
void hexToAscii(const String& hexString, String& asciiString) {
  int stringLength = hexString.length();  // Get the length of the hexadecimal string
  asciiString = "";  // Initialize the ASCII string as an empty string

  // Iterate over the hexadecimal string, processing two characters at a time
  for (int i = 0; i < stringLength; i += 2) {
    // Extract two characters from the hexadecimal string
    String hexByte = hexString.substring(i, i + 2);

    // Convert the two-character hexadecimal representation to decimal
    // strtol converts the hexadecimal string to an integer using base 16 (hexadecimal)
    // hexByte.c_str() returns a C-style string (const char*) required by strtol
    // The result is stored in decimalValue
    int decimalValue = strtol(hexByte.c_str(), NULL, 16);
  
    // Append the corresponding ASCII character to the ASCII string
    // (char)decimalValue converts the decimal value to its corresponding ASCII character
    // The character is appended to the ASCII string using the += operator
    asciiString += (char)decimalValue;
  }
}
