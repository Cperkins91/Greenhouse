#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

//Initialize variables
  #define relay 14 //D5
  unsigned long StartTime = millis(); //Start time used to calculate when to publish
  String message="";
  bool flag = false;
  bool DEBUG = true;
//Variables for publishing
  char luxChar[50] ;
  char tempChar[50];
  String tempString;
   
  void callback(char* topic, byte* payload, unsigned int length);
  void TestConnection();
  void wifiConnect();
  void mqttConnect();
  void sendLux();

// Wifi Settings
  const char* ssid = "memashouse";
  const char* password = "Hobart10";
  
//MQTT Settings
  const char* mqttServer = "192.168.1.249";
  const int mqttPort = 1883;
  const char* mqttUser = "ESP_LIGHT_CONTROLLER";
  const char* mqttPassword = "";
  const char* MqttTopic = "Light_Control";
  WiFiClient espClient;
  PubSubClient client(espClient);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Setup I2C connection for BH1750
  Wire.begin();
  lightMeter.begin();
  if (DEBUG) {
    Serial.println("BH170 Initialized");
  }
  wifiConnect();
  mqttConnect();  
  pinMode(relay, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (DEBUG){
  //Serial.print("Testing for message data:");
  //Serial.println(message);
  }
  testConnection();
  if (message == "ON" && flag == true) {
    Serial.println("Turned on the light");
    digitalWrite(relay, HIGH);
    flag  = false; //flag will flip during callback function
    message = "";
  }
  else if (message == "OFF" && flag == true) {
    Serial.println("Turned off the lights");
    flag = false; //flag will flip during callback function
    digitalWrite(relay, LOW);
    message = "";
  }   
  if (millis() - StartTime > 10000 || millis()-StartTime < 1){// Check for 10sec past and retrieve and publish data
      StartTime = millis();
      sendLux();
      delay(50);

  }  
  client.loop();
}

void wifiConnect() {
    //Establish WiFi Connection
    WiFi.begin(ssid, password);
    Serial.println("Attemping to Connect to WiFI...");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting....");
    }
    Serial.println("WiFi Connection Successful");
    client.setServer(mqttServer,mqttPort);
    client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message = message + (char)payload[i];
    flag = true; // flip flag to true allowing light control conditional to pass. This means the light will only change when there is a NEW message.
  }
 
  Serial.println();
  Serial.println("-----------------------");
}
void mqttConnect() { //Initiate MQTT Server Connection
    while(!client.connected()){
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP8266Client_Lights",mqttUser,mqttPassword)){
            Serial.println("MQTT Connection Successful");
            client.subscribe(MqttTopic);

        }
        else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void testConnection() { //Check for connection loss and reconnect if so
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost");
        wifiConnect();
    }
    if (!client.connected()){ //check for connection loss
        Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
        mqttConnect();
    } //check for connection loss
}

void sendLux(){
    tempString = String(lightMeter.readLightLevel());
    tempString.toCharArray(tempChar, tempString.length()+1); // Create Char array
    client.publish("Lux_Outdoors" , tempChar);
    if (DEBUG){
    Serial.print("Publishing Light Data (lux) " );
    Serial.println(tempChar);
    }

}
