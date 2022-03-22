 #include <ESP8266WiFi.h>
 #include <PubSubClient.h>
 #include <String.h>
  
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

void setup() {
// put your setup code here, to run once:
  Serial.begin(9600);
  
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

int lux = 280;
char luxChar[50];
String luxString;
long StartTime = millis();
void loop() {
  // put your main code here, to run repeatedly:
 // luxString = String(lux);
  if (Serial.available ()){ //Reading the Serial port if data is available
      while (Serial.available()){
        delay(2);
        char c = Serial.read();
        luxString += c; //append next character to string
      }
      luxString.toCharArray(luxChar, luxString.length()+1);
      Serial.println("LuxString = " + luxString);
      Serial.println("Publishing Data");
      Serial.println (luxChar);
      client.publish("Lux_TEST" , luxChar);
      luxString = "";
  }
  luxString.toCharArray(luxChar, luxString.length()+1);
  if (millis() - StartTime > 120000){
    StartTime = millis();
    luxString = String(lux); // Set lux string to the defined int
    luxString.toCharArray(luxChar, luxString.length()+1); // Create Char array
    Serial.println("LuxString = " + luxString);
    Serial.println("Publishing Data");
    Serial.println (luxChar);
    client.publish("Lux_TEST" , luxChar);
  }
  /*
  if (client.publish("Lux_TEST", LuxString)){
    Serial.print("Data Publish Complete");
  }
  else {Serial.print("Publication Failed")}
  */
  //delay(10000);
  //lux += 10;
  //client.publish("Lux_TEST", "330");

}
