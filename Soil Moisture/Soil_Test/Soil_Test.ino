//#include <ESP8266WiFi.h>
//#include <Wire.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

// Setup temperature probes

// Setup moisture probes
const int analogInPin1 = 21; // Pin 5 onboard
const int analogInPin2 = 32; //Pin 7 onboard
//
//
int sensorValue = 0;  // value read from the pot
float Moisture1, Moisture2;

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);
//  sensors.begin();

 // DS18B20.begin();
}

void loop() {
  // read the analog in value
  int sensorValue1 = analogRead(analogInPin1);
 
  // print the readings in the Serial Monitor
  Serial.print("sensor1 = ");
  Serial.println(sensorValue1);
  Moisture1 = (1.0 - ((float(sensorValue1) - 330)/335))*100.0;
  Serial.print("MoisturePercentage: ");
  Serial.println(Moisture1);
  delay(1000);
// read the analog in value
//int  sensorValue2 = analogRead(analogInPin2);
// 
//  // print the readings in the Serial Monitor
//  Serial.print("sensor2 = ");
//  Serial.println(sensorValue2);
//  Moisture2 = (1.0 - ((float(sensorValue2) - 330)/335))*100.0;
//  Serial.print("MoisturePercentage: ");
//  Serial.println(Moisture2);
}

// 100% = 330
// 0% = 665
