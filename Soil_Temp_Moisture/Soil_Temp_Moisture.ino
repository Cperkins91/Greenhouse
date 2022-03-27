#include <WiFi.h>
//#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <String.h>

void setup_wifi();
void mqttConnect ();
void callback(char* topic, byte* payload, unsigned int length);

void sendMoisture();
void sendSoilTemp();

//Wifi setup
const char* ssid = "memashouse";
const char* password = "Hobart10";

//MQTT server setup
const char* mqttServer = "192.168.1.249";
const int mqttPort = 1883;
const char* mqttUser = "ESP_Soil";
const char* mqttPassword = "";
WiFiClient espClient;
PubSubClient client(espClient);


// Setup temperature probes
#define ONE_WIRE_BUS 21
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = {0x28, 0x60, 0x2E, 0x49, 0xF6, 0x9F, 0x3C, 0x6E};
DeviceAddress sensor2 = {0x28, 0xE9, 0x52, 0x96, 0xF0, 0x1, 0x3C, 0xB};

// Setup moisture probes
#define analogInPin1 32 // Pin 5 onboard
#define analogInPin2 34 //Pin 7 onboard
 // Callibration Data
 // Probe 1: 0% = 2540 100% = 1055
 // Probe 2: 0% = 2600 100% = 1090
const int probeOneMax = 2540; // Max ADC output of probe
const int probeOneMin = 1055; // Min ADC output of probe
const int probeTwoMax = 2600;
const int probeTwoMin = 1090;
//float Moisture; MOVED TO FUNCTION
// Setup mqtt publishing variables
String tempString; //Temp String for converting to char*
char tempChar[50]; // Temp char* to publish to mqtt
//int sensorValue = 0;  // MOVED TO FUNCTION



unsigned long StartTime = millis(); //Start time used to calculate when to publish
bool DEBUG = true;
void setup() {
    // initialize serial communication at 115200
    Serial.begin(115200);
    setup_wifi();
    mqttConnect();
    sensors.begin();

    // DS18B20.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost");
        setup_wifi();
    }
    
    if (!client.connected()){ //check for connection loss
        Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
        Serial.println("Inside loop check for connection loss");
        //Serial.println(client.connected());
        mqttConnect();
    } //check for connection loss

    if (millis() - StartTime > 10000 || millis()-StartTime < 1){// Check for 10sec past and retrieve and publish data
        StartTime = millis();
        sendMoisture();
        sendSoilTemp();
        delay(50);

    }

    client.loop();
}
//
// 100% = 1050
// 0% = 2600
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
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void mqttConnect() { //Initiate MQTT Server Connection
    client.setServer(mqttServer,mqttPort);
    client.setCallback(callback);
    while(!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP32Client",mqttUser,mqttPassword)){
            Serial.println("MQTT Connection Successful");
            delay(200);
            //client.subscribe("inTopic");

        }
        else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
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

void sendMoisture() {
    int sensorValue=0;
    float Moisture = 0.0;
    sensorValue = analogRead(analogInPin1);
    Moisture = (1.0 - ((float(sensorValue) - probeOneMin) / (probeOneMax-probeOneMin))) * 100.0;
    tempString = String(Moisture);
    tempString.toCharArray(tempChar, tempString.length() + 1);
    client.publish("Moisture_Probe_1" , tempChar);
    if (DEBUG) {
        Serial.print("Publishing Soil Moisture 1 Content (%) ");
        Serial.println(Moisture);
        Serial.print("Callibration data:");
        Serial.println(sensorValue);
    }
    sensorValue = analogRead(analogInPin2);
    Moisture = (1.0 - ((float(sensorValue) - probeTwoMin) / (probeTwoMax-probeTwoMin))) * 100.0;
    tempString = String(Moisture);
    tempString.toCharArray(tempChar, tempString.length() + 1);
    client.publish("Moisture_Probe_2" , tempChar);
    if (DEBUG) {
        Serial.print("Publishing Soil Moisture 2 Content (%) ");
        Serial.println(Moisture);
        Serial.print("Callibration Data:");
        Serial.println(sensorValue);
    }
}
 // Callibration Data
 // Probe 1: 0% = 2540 100% = 1055
 // Probe 2: 0% = 2600 100% = 1090
void sendSoilTemp() {
    sensors.requestTemperatures();
    float sensor1Temp = 0;
    float sensor2Temp = 0;

    if (DEBUG) {
        Serial.println("Requesting Temps \n");
    }
    sensor1Temp = sensors.getTempC(sensor1);
    tempString = String(sensor1Temp);
    tempString.toCharArray(tempChar, tempString.length()+1);
    //tempString = String(sensor1Temp);
    if (DEBUG) {
        Serial.print("Sensor 1: ");
        Serial.println(sensor1Temp);
        Serial.println("Publishing Data for temperature sensor 1");
    }
    client.publish("Soil_Temp_1", tempChar);
    sensor2Temp = sensors.getTempC(sensor2);
    tempString = String(sensor2Temp);
    tempString.toCharArray(tempChar, tempString.length()+1);
    if (DEBUG) {
        Serial.print("Sensor 2:");
        Serial.println(sensor2Temp);
        Serial.println("Publishing Data for temperature sensor 2...");
    }
    client.publish("Soil_Temp_2",tempChar);


}
