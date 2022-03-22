 #include <ESP8266WiFi.h>
 const char* ssid = "memashouse";
 const char* password = "Hobart10";

void setup() {
  // put your setup code here, to run once:
// put your setup code here, to run once:
  Serial.begin(115200);
  
//Establish WiFi Connection
  WiFi.begin(ssid, password);
  Serial.println("Attemping to Connect to WiFI...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting....");
 }
  Serial.println("WiFi Connection Successful");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Idle in Loop");
  delay(5000);
} 
