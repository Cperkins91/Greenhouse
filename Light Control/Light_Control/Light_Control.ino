 #include <ESP8266WiFi.h>
 #include <PubSubClient.h>
 #include <String.h>
 #define relay 5
 String message="";
 bool flag = false;
 bool DEBUG = false;
 const char* MqttTopic = "Fan Control_TEST";
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message = message + (char)payload[i];
    flag = true;
  }
 
  Serial.println();
  Serial.println("-----------------------");
}

void wifiConnect();
void mqttConnect();

  const char* ssid = "memashouse";
  const char* password = "Hobart10";

  const char* mqttServer = "192.168.1.249";
  const int mqttPort = 1883;
  const char* mqttUser = "ESP_LIGHT_CONTROLLER";
  const char* mqttPassword = "";

  WiFiClient espClient;
  PubSubClient client(espClient);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  wifiConnect();
  mqttConnect();  
  pinMode(relay, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (DEBUG){
  Serial.print("Testing for message data:");
  Serial.println(message);
  }
  Test_connection();
  if (message == "ON" && flag == true) {
    Serial.println("Turned on the light");
    digitalWrite(relay, HIGH);
    flag  = false;
    message = "";
  }
  else if (message == "OFF" && flag == true) {
    Serial.println("Turned off the lights");
    flag = false;
    digitalWrite(relay, LOW);
    message = "";
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

void Test_Connection() { //Check for connection loss and reconnect if so
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost");
        wifiConnect();
    }
    if (!client.connected()){ //check for connection loss
        Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
        mqttConnect();
    } //check for connection loss
}
