/*
 * DewDog
 * (c) 2016 jtr
 * ???
 */

#include "DHT.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define DHT1PIN 2    // modify to the pin we connected
#define DHT2PIN 3    // modify to the pin we connected
#define DHT3PIN 4    // modify to the pin we connected

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

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

void setup() {

  Serial.begin(9600); 
  Serial.println("*** Arduino");
  Serial.println("*** Dew Dog");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");

  dht1.begin();
  dht2.begin();
  dht3.begin();

  if (!bmp.begin()) {
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
  }  
}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float d1 = t1 - ((100 - h1) / 5);

  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  float d2 = t2 - ((100 - h2) / 5);

  float h3 = dht3.readHumidity();
  float t3 = dht3.readTemperature();
  float d3 = t3 - ((100 - h3) / 5);
  
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t1) || isnan(h1)) {
    Serial.println("Failed to read from DHT1");
  } else {
    Serial.println(String(++counter) + ".");
    Serial.print("Humidity-1: "); 
    Serial.print(h1);
    Serial.print(" %\t");
    Serial.print("Temperature-1: "); 
    Serial.print(t1);
    Serial.print(" C\t");
    Serial.print("DewPoint-1: "); 
    Serial.print(d1);
    Serial.println(" C");
  }
  if (isnan(t2) || isnan(h2)) {
    Serial.println("Failed to read from DHT2");
  } else {
    Serial.print("Humidity-2: "); 
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature-2: "); 
    Serial.print(t2);
    Serial.print(" C\t");
    Serial.print("DewPoint-2: "); 
    Serial.print(d2);
    Serial.println(" C");
  }

  if (isnan(t3) || isnan(h3)) {
    Serial.println("Failed to read from DHT3");
  } else {
    Serial.print("Humidity-3: "); 
    Serial.print(h3);
    Serial.print(" %\t");
    Serial.print("Temperature-3: "); 
    Serial.print(t3);
    Serial.print(" C\t");
    Serial.print("DewPoint-3: "); 
    Serial.print(d3);
    Serial.println(" C");  
}

  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure) {
    Serial.print("Pressure:    ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
    
    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  } else {
    Serial.println("BMP sensor error");
  }  

  Serial.println("");

  for (int n = 1; n <= 1; ++n)
    delay(10 * DELAY_SECOND);

  if (counter == RESET_COUNTER) {
    Serial.println("Rebooting....");
    delay(DELAY_SECOND);
    resetFunc();
  }
}
