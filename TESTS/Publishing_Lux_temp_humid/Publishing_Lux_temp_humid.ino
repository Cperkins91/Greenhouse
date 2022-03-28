#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <BH1750.h>
#include <Wire.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 14 //D5 onboard
DHT dht(DHTPIN,DHTTYPE);
BH1750 lightMeter;
void wifiConnect();
void mqttConnect();
void callback(char* topic, byte* payload, unsigned int length);

const char* ssid = "memashouse";
const char* password = "Hobart10";

const char* mqttServer = "192.168.1.249";
const int mqttPort = 1883;
const char* mqttUser = "ESP_1";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

float lux;
char luxChar[50] ;
char tempChar[50];
String tempString;
String tempString;
unsigned long StartTime = millis(); //Start time used to calculate when to publish

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
    // put your main code here, to run repeatedly:
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost");
        wifiConnect();
    }
    if (!client.connected()){ //check for connection loss
        Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
        mqttConnect();
    } //check for connection loss

    if (millis() - StartTime > 10000 || millis()-StartTime < 1){// Check for 10sec past and retrieve and publish data
        StartTime = millis();
        /*
        luxString = String(lightMeter.readLightLevel());
        luxString.toCharArray(luxChar, luxString.length()+1); // Create Char array
        Serial.println("LuxString = " + luxString);
        Serial.println("Publishing Light Data");
        Serial.println (luxChar);
        client.publish("Lux_TEST" , luxChar);
        delay(50);
        Serial.print("Temperature in Farenheight ");
        Serial.println(dht.readTemperature(true));
        Serial.print("Humidity ");
        Serial.println(dht.readHumidity()); // Get humidity
        Serial.print("Heat Index ");
        Serial.println(dht.computeHeatIndex(dht.readTemperature(true),dht.readHumidity())); // Get heat index
        */
        sendHumidity();
        sendLux();
        sendTempC();
        delay(50)

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
void sendTempC (){
    tempString = String(dht.readTemperature());
    tempString.toCharArray(tempChar, tempString.length()+1);
    Serial.print("Publishing Temperature data ", tempChar, " degrees C. \n");
    //Serial.println(tempChar);
    client.publish("Temp_TEST", tempChar);
}
void sendHumidity(){
    tempString = dht.readHumidity());
    tempString.toCharArray(tempChar, tempString.length()+1);
    Serial.print("Publishing Humidity data ",tempChar, "% \n" );
    //Serial.println(tempChar);
    client.publish("Humidity_TEST", tempChar);
}
void sendLux()){
    tempString = String(lightMeter.readLightLevel());
    tempString.toCharArray(luxChar, tempString.length()+1); // Create Char array
    Serial.println("Publishing Light Data", tempChar, "lux \n" );
    client.publish("Lux_TEST" , tempChar);

}
