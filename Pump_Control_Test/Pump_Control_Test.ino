/* Created by Christopher Perkins
 *  4/20/2022
 *  This code is used to program an ESP 8266 to build an a Irrigation Controller. 
 *  In this form it will connect to the local wifi, establish an mqtt session,
 *  and and check for messages on the given MQTT topic. The loop function will test 
 *  the WiFi and MQTT connection, and wait for a message. If the ESP recieves a 
 *  message it will save it to the message string and check it in the loop. If
 *  it is the ON message it will enter the irrigation control conditional. Inside the conditonal,
 *  the ESP will first turn on the pump, wait 1 sec, open the solenoid, then 
 *  wait for the specified amount of time, close the solenoid, wait 250ms, and close the 
 *  pump. The 1 second delay is so that inrush current between the pump and solenoid occur at slightly different 
 *  times. The is to reduce stress on the PCB traces.
 */
 
 #include <ESP8266WiFi.h>
 #include <PubSubClient.h>
 #include <String.h>
#define Pump 5 // GPIO D1 
#define Relay 4 //GPIO D2
String message="";
bool flag = false; //Flag so that only the irrigation only turns on once until the esp recieves another message
bool DEBUG = false;
long StartTime = 0;
int IrrigationLength = 15000;
void callback(char* topic, byte* payload, unsigned int length) { //Function to recieve MQTT messages
  message = "";
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

void wifiConnect();
void mqttConnect();

  const char* ssid = "memashouse";
  const char* password = "Hobart10";

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
 if (DEBUG){
  Serial.println("Entered loop");
 }
 Test_Connection();
 if (message == "ON"){
  Serial.println("Irrigation turning on... \n Turning on the pump");
  digitalWrite(Pump, HIGH);
  delay (1000);
  StartTime = millis();
  Serial.println("Opening the solenoid");
  while (millis() - StartTime < IrrigationLength && message == "ON") {
    digitalWrite(Relay,HIGH);
    digitalWrite(Pump, HIGH);
    yield(); //Must yield to reset watchdog timer so it doesn't reset ESP
  }
  Serial.println("Irrigation cycle complete! \n Turning off the pump and closing solenoid");
  digitalWrite(Relay, LOW);
  delay (250);
  digitalWrite(Pump, LOW);
  flag = false;
  message = "";
 }
 client.loop();
}

void Test_Connection() { //Check for connection loss and reconnect if so
  if (DEBUG){
    Serial.println("Entered Test_Connection function");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
    wifiConnect();
  }
  if (!client.connected()){ //check for connection loss
    Serial.println("Connection to MQTT Broker interrupted \n Attempting to Reconnect... ");
    mqttConnect();
  } 
}
void wifiConnect() {  //Establish WiFi Connection
  if (DEBUG){
    Serial.println("Entered WifiConnect function");
    }
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
  if (DEBUG){
    Serial.println("Entered mqttConnect function");
  }
  while(!client.connected()){
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqttUser)){
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
