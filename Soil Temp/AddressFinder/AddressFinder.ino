//
//#include <OneWire.h>
//
//OneWire ds(21);
//
//void setup() {
//  serial.begin(9600);
//
//}
//
//void loop(void) {
//  byte i;
//  byte present = 0;
//  byte data[12];
//  byte addr[8];
//
//  if (!ds.search(addr)) {
//    Serial.print("No more addresses!\n");
//    ds.reset_search();
//    delay(250);
//    return;
//  }
//
//}

/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
//
//#include <OneWire.h>
//
//// Based on the OneWire library example
//
//OneWire ds(21);  //data wire connected to GPIO 10
//
//void setup(void) {
//  Serial.begin(9600);
//}
//
//void loop(void) {
//  byte i;
//  byte addr[8];
//  
//  if (!ds.search(addr)) {
//    Serial.println(" No more addresses.");
//    Serial.println();
//    ds.reset_search();
//    delay(250);
//    return;
//  }
//  Serial.print(" ROM =");
//  for (i = 0; i < 8; i++) {
//    Serial.write(' ');
//    Serial.print(addr[i], HEX);
//  }
//}

/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-temperature-sensor
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PIN1  15 // ESP32 pin GIOP21 connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN1);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

void setup() {
  Serial.begin(9600); // initialize serial
  DS18B20.begin();    // initialize the DS18B20 sensor
}

void loop() {
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32; // convert °C to °F

  Serial.print("Temperature: ");
  Serial.print(tempC);    // print the temperature in °C
  Serial.print("°C");
  Serial.print("  ~  ");  // separator between °C and °F
  Serial.print(tempF);    // print the temperature in °F
  Serial.println("°F");

  delay(500);
}
