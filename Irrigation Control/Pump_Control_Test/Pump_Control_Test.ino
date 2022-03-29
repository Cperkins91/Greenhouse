#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#define Pump 5 // GPIO D1 
#define Relay 4 //GPIO D2
String message = "";
bool flag = false; //Flag so that only the irrigation only turns on once until the esp recieves another message
bool DEBUG = true;

// Counter for keeping 15sec measurement intervals
long StartTime = 0;


void callback(char* , byte*, unsigned int);
void wifiConnect();
void mqttConnect();
//wifi settings
const char* ssid = "memashouse";
const char* password = "Hobart10";
// mqtt settings
const char* mqttServer = "192.168.1.249";
const int mqttPort = 1883;
const char* mqttUser = "ESP_IRRIGATION_CONTROLLER";
const char* mqttPassword = "";
const char* MqttTopic = "Irrigation Control";
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  pinMode(Pump, OUTPUT);
  pinMode(Relay, OUTPUT);
  wifiConnect();
  mqttConnect();
}

void loop() {
  if (DEBUG) {
    Serial.println("Entered loop");
  }
  Test_Connection();
  if (message == "ON" && flag == true) {
    Serial.println("Irrigation turning on... \n Opening the solenoid");
    digitalWrite(Relay, HIGH);
    delay (1000);
    StartTime = millis();
    Serial.println("Turning on the pump");
    while (millis() - StartTime < 15000 && message == "ON") {
      digitalWrite(Pump, HIGH);
      yield(); //Must yield to reset watchdog timer so it doesn't reset ESP
    }
    Serial.println("Irrigation cycle complete! \n Turning off the pump and closing solenoid");
    digitalWrite(Relay, LOW);
    delay (100);
    digitalWrite(Pump, LOW);
    flag = false;
    message = "";
  }
  client.loop();
}

void Test_Connection() { //Check for connection loss and reconnect if so
  if (DEBUG) {
    Serial.println("Entered Test_Connection function");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
    wifiConnect();
  }
  if (!client.connected()) { //check for connection loss
    Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
    mqttConnect();
  } //check for connection loss
}
void wifiConnect() {  //Establish WiFi Connection
  if (DEBUG) {
    Serial.println("Entered WifiConnect function");
  }
  WiFi.begin(ssid, password);
  Serial.println("Attemping to Connect to WiFI...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting....");
  }
  Serial.println("WiFi Connection Successful");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}
void mqttConnect() { //Initiate MQTT Server Connection
  if (DEBUG) {
    Serial.println("Entered mqttConnect function");
  }
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client_Lights", mqttUser, mqttPassword)) {
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
void callback(char* topic, byte* payload, unsigned int length) { //Function to recieve MQTT messages

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message = message + (char)payload[i];
    flag = true; // A message has been recieved
  }

  Serial.println();
  Serial.println("-----------------------");
}
