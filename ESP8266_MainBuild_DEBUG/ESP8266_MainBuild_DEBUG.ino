/* Created by Christopher Perkins
 *  4/20/2022
 *  This code is used to program an ESP 8266 to build an ambient sensor suite. 
 *  In this form it will connect to the local wifi, establish an mqtt session,
 *  and read a BH1750 and DHT21 sensor. After reading it will publish the readings
 *  to the mqtt broker under discrete topics. The loop function will test the WiFi and MQTT connection, 
 *  check for amount of time passed since the last measurment, and if past the Period set, it will take
 *  a new measurement and publish it the the respective topic.
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <BH1750.h>
#include <Wire.h>
#include "DHT.h"

#define DHTTYPE DHT21
#define DHTPIN 13 //D7 onboard
DHT dht(DHTPIN,DHTTYPE,30);
BH1750 lightMeter;
void wifiConnect();
void mqttConnect();
void testConnection();
void callback(char* topic, byte* payload, unsigned int length);

  //Debug
  const bool DEBUG = true;
  
  //Wifi setup
  const char* ssid = "memashouse";
  const char* password = "Hobart10";
  
  //MQTT server setup
  const char* mqttServer = "192.168.1.249";
  const int mqttPort = 1883;
  const char* mqttUser = "ESP_1";
  const char* mqttPassword = "";
  
  //Soil Moisture Setup
  const int analogInPin = A0;
  
  WiFiClient espClient;
  PubSubClient client(espClient);
  
  
  //Variables for publishing
  char luxChar[50] ;
  char tempChar[50];
  String tempString;
  
  unsigned long StartTime = millis(); //Start time used to calculate when to publish
  int Period = 10000; // set this to the length between each publication you want. (In milliseconds)
  // Moisture meter variables
  float Moisture;
  int sensorValue = 0;
  
  //Light Variable
  float lux;
void setup() {
// put your setup code here, to run once:
    Serial.begin(9600);
    dht.begin();
// Setup I2C Connection
    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 initialized"));
    wifiConnect();
    mqttConnect();
}

void loop() {

  testConnection();
    if (millis() - StartTime > Period || millis()-StartTime < 1){// Check for 10sec past and retrieve and publish data
        StartTime = millis();
        sendHumidity();
        sendLux();
        sendTempC();
        sendMoisture();
        delay(50);

    }
    client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }

    Serial.println();
    Serial.println("-----------------------");

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
void mqttConnect() { //Initiate MQTT Server Connection
    while(!client.connected()){
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP8266Client",mqttUser,mqttPassword)){
            Serial.println("MQTT Connection Successful");
            client.subscribe("inTopic");

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

void sendTempC (){
    tempString = String(dht.readTemperature());
    tempString.toCharArray(tempChar, tempString.length()+1);
    if (DEBUG){
    Serial.print("Publishing Temperature data (degrees C): ");
    Serial.println(tempChar);
    }
    client.publish("Temp_TEST", tempChar);
}
void sendHumidity(){
    tempString = String(dht.readHumidity());
    tempString.toCharArray(tempChar, tempString.length()+1);
    if (DEBUG){
    Serial.print("Publishing Humidity data (%) " );
    Serial.println(tempChar);
    }
    client.publish("Humidity_TEST", tempChar);
}
void sendLux(){
    tempString = String(lightMeter.readLightLevel());
    tempString.toCharArray(tempChar, tempString.length()+1); // Create Char array
    client.publish("Lux_Indoors" , tempChar);
    if (DEBUG){
    Serial.print("Publishing Light Data (lux) " );
    Serial.println(tempChar);
    }

}
void sendMoisture() {
  sensorValue = analogRead(analogInPin);
  Moisture = (1.0 - ((float(sensorValue) - 330)/335))*100.0;
  tempString = String(Moisture);
  tempString.toCharArray(tempChar, tempString.length()+1);
  client.publish("Moisture_TEST" , tempChar);
  if (DEBUG){
      Serial.print("Publishing Soil Moisture Content (%) ");
      Serial.println(Moisture);
  }
}
