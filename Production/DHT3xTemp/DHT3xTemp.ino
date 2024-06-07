/*
 * DHT3xTemp
 * (c) 2016 jtr
 * Nano
 */

#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define SERIALNO "01"

#define DHT1PIN 8    // modify to the pin we connected
#define DHT2PIN 9    // modify to the pin we connected
#define DHT3PIN 10    // modify to the pin we connected

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);
DHT dht3(DHT3PIN, DHTTYPE);

int counter = 0;

void setup() {

  Serial.begin(9600); 

  dht1.begin();
  dht2.begin();
  dht3.begin();

  Serial.println("start");

}

void readSensors(int n)
{
  float h1, h2, h3;
  float t1, t2, t3;
  float d1, d2, d3;

  switch (n) {
    
  case 1:

    h1 = dht1.readHumidity();
    t1 = dht1.readTemperature();
    d1 = t1 - ((100 - h1) / 5);

    if (isnan(t1) || isnan(h1)) {
      Serial.println("Failed to read from DHT1");
    } else {
      Serial.print("Hum1 = "    + String(h1) + " %   "); 
      Serial.print("Tmp1 = "    + String(t1) + " C   "); 
      Serial.println("Dew1 = " + String(d1) + " C"); 
    }
    break;

  case 2:

    h2 = dht2.readHumidity();
    t2 = dht2.readTemperature();
    d2 = t2 - ((100 - h2) / 5);
  
    if (isnan(t2) || isnan(h2)) {
      Serial.println("Failed to read from DHT2");
    } else {
      Serial.print("Hum2 = "    + String(h2) + " %   "); 
      Serial.print("Tmp2 = "    + String(t2) + " C   "); 
      Serial.println("Dew2 = " + String(d2) + " C"); 
    }
    break;

  case 3:

    h3 = dht3.readHumidity();
    t3 = dht3.readTemperature();
    d3 = t3 - ((100 - h3) / 5);

    if (isnan(t3) || isnan(h3)) {
      Serial.println("Failed to read from DHT3");
    } else {
      Serial.print("Hum3 = "    + String(h3) + " %   "); 
      Serial.print("Tmp3 = "    + String(t3) + " C   "); 
      Serial.println("Dew3 = " + String(d3) + " C"); 
    }
    break;
    
  }
}

void loop() {
    
  if (Serial.available()) {    
    char input = Serial.read();  
    if (input != -1) {
      switch (input) {
      case 'r':
        ++counter;
        if (counter == 1) {
          readSensors(1);
        } else if (counter == 2) {
          readSensors(2);
        } else if (counter == 3) {
          readSensors(3);
          counter = 0;
        }
        break;
      }
    }
  }
  delay(100);
}

