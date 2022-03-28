 #include <ESP8266WiFi.h>
 #include <PubSubClient.h>
 #include <String.h>
 #include <BH1750.h>
 #include <Wire.h>
 BH1750 lightMeter;
  
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
  const char* ssid = "memashouse";
  const char* password = "Hobart10";

  const char* mqttServer = "192.168.1.249";
  const int mqttPort = 1883;
  const char* mqttUser = "ESP_1";
  const char* mqttPassword = "";

  WiFiClient espClient;
  PubSubClient client(espClient);

  float lux;
  char luxChar[50];
  String luxString;
  long StartTime = millis(); //Start time used to calculate when to publish

void setup() {
// put your setup code here, to run once:
  Serial.begin(9600);
// Setup I2C Connection
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 initialized"));

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
  
//Initiate MQTT Server Connection
  while(!client.connected()){
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client",mqttUser,mqttPassword)){
      Serial.println("MQTT Connection Successful");
    }
    else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
    if (client.connect("ESP8266Client",mqttUser,mqttPassword)){
      Serial.println("MQTT Connection Successful");
    }
    else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  if (millis() - StartTime > 12000){
    StartTime = millis();
    luxString = String(lightMeter.readLightLevel());
    luxString.toCharArray(luxChar, luxString.length()+1); // Create Char array
    Serial.println("LuxString = " + luxString);
    Serial.println("Publishing Data");
    Serial.println (luxChar);
    client.publish("Lux_TEST" , luxChar);
  }
}
