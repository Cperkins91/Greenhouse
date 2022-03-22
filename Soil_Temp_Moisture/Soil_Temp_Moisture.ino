#include <OneWire.h>
#include <DallasTemperature.h>

bool DEBUG = false;

// Setup temperature probes
#define ONE_WIRE_BUS 21
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = {0x28, 0x60, 0x2E, 0x49, 0xF6, 0x9F, 0x3C, 0x6E};
DeviceAddress sensor2 = {0x28, 0xE9, 0x52, 0x96, 0xF0, 0x1,0x3C, 0xB};

// Setup moisture probes
#define analogInPin1 32 // Pin 5 onboard
//const int analogInPin2 = 32; //Pin 7 onboard
const int probeOneMax = 2600; // Max ADC output of probe
const int probeOneMin = 1050; // Min ADC output of probe
int sensorValue = 0;  // value read from the moisture probe
float Moisture = 0; // Normalized value

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  // read the analog in value
sensorValue = analogRead(analogInPin1);

// print the readings in the ADC
if (DEBUG) {
Serial.print("sensor = ");
Serial.println(sensorValue);
}
Moisture = (1.0 - ((float(sensorValue) - probeOneMin)/(probeOneMax - probeOneMin)))*100.0; //Calibration equation 
Serial.print("MoisturePercentage: ");
Serial.println(Moisture);
delay(1000);
Serial.print("Requesting Temps \n");
sensors.requestTemperatures();
Serial.print("Sensor 1: ");
Serial.print(sensors.getTempF(sensor1));
Serial.print("\n Sensor 2: ");
Serial.println(sensors.getTempF(sensor2));
delay(2000);
}

// 100% = 1050
// 0% = 2600
