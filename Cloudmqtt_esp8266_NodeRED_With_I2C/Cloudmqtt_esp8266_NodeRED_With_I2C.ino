/*****
 
 All the resources for this project:
 http://randomnerdtutorials.com/
 
*****/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "SmartisanXi";
const char* password = "82517802";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
//const char* mqtt_server = "127.0.0.1";
const char* mqttServer = "m10.cloudmqtt.com";
const int mqttPort = 18647;
const char* mqttUser = "xjerpjsc";          //use to access the mqtt account
const char* mqttPassword = "Fg5cFpOtwf2F";  //^^^^^^^^^^^^^^^^^^^^^^^^^^^
WiFiClient espClient;
PubSubClient client(espClient);

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
//WiFiClient XDMespClient;
//PubSubClient client(XDMespClient);



// Don't change the function below. This functions connects your ESP8266 to your router
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


//In "Callback" function, ESP listening for command via WIFI
// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something

void initialise_MQTT() {

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
      //strip.setPixelColor(0, 0, 255, 0); // Green, means connected to MQTT server
      //strip.show();
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      //strip.setPixelColor(0, 255, 255, 0); // Yellow means MQTT connection issue
      //strip.show();
      delay(2000);
    }
  }
}

//test whether callback works when free!!
//void callback(String topic, byte* message, unsigned int length) {
//  Serial.print("Message arrived on topic: ");
//  Serial.print(topic);
//  Serial.print(". Message: ");
//  String messageTemp;
//  
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)message[i]);
//    messageTemp += (char)message[i];
//  }
//  Serial.println();
//
//  // Feel free to add more if statements to control more GPIOs with MQTT
//
//  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
//  if(topic=="room/lamp"){
//      Serial.print("Changing Room lamp to ");
//      if(messageTemp == "on"){
//        digitalWrite(lamp, HIGH);
//        Serial.print("On");
//      }
//      else if(messageTemp == "off"){
//        digitalWrite(lamp, LOW);
//        Serial.print("Off");
//      }
//  }
//  Serial.println();
//}



// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  client.setServer(mqttServer, mqttPort);
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
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("room/lamp");
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
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  
  Serial.begin(115200);
  setup_wifi();
  initialise_MQTT();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}


void loop() {

   float i2c_receive;
   static char temperatureTemp[7];
   dtostrf(i2c_receive, 2, 0, temperatureTemp);
    
  if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("ESP8266Client");

    Wire.requestFrom(8, 1);// request from address 8 and data size is 1 byte
    while(Wire.available())
      {
        i2c_receive = Wire.read();
        Serial.println(i2c_receive);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN,LOW);
        delay(40);
      }
      
    // Publishes Temperature value
    //PUBLISHING!!
    client.publish("room/temperature", temperatureTemp);

} 
