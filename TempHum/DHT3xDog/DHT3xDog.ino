/*
 * DHT3xDog
 * (c) 2017 jtr
 * UNO R3
 */

#include "DHT.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

/* Auriga */
#define SERIALNO "01"

#define DHT1PIN 4    // modify to the pin we connected
#define DHT2PIN 2    // modify to the pin we connected
#define DHT3PIN 3    // modify to the pin we connected

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

#define RESET_COUNTER 1000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

float t1min, t2min, t3min;
float t1max, t2max, t3max;

float h1min, h2min, h3min;
float h1max, h2max, h3max;

String fTemp(float f)
{
  if (f >= 10.0) {
    return (" " + String(f));
  } else if (f <= -10.0) {
    return (String(f));
  } else if (f >= 0.0) {
    return ("  " + String(f));
  } else if (f < 0.0) {
    return (" " + String(f));
  } else {
    return (String("NA"));
  }
}

String fHumi(float f)
{
  if (f >= 10.0) {
    return (String(f));
  } else {
    return (" " + String(f));
  }
}

void EEPROM_save()
{
  int eeAddress = 0;

  EEPROM.put(eeAddress, t1max);
  eeAddress += sizeof(float);

  EEPROM.put(eeAddress, t2max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, t3max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, t1min);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, t2min);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, t3min);
  eeAddress += sizeof(float);

  EEPROM.put(eeAddress, h1max);
  eeAddress += sizeof(float);

  EEPROM.put(eeAddress, h2max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, h3max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, h1min);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, h2min);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, h3min);
  eeAddress += sizeof(float);

}

void EEPROM_restore()
{ 
  int eeAddress = 0;
  
  EEPROM.get(eeAddress, t1max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, t2max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, t3max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, t1min);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, t2min);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, t3min);
  eeAddress += sizeof(float);

  EEPROM.get(eeAddress, h1max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, h2max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, h3max);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, h1min);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, h2min);
  eeAddress += sizeof(float);
  
  EEPROM.get(eeAddress, h3min);
  eeAddress += sizeof(float);

}

void EEPROM_clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void showhelp()
{
  Serial.println("Press z to reset min/max values, r for reboot and h for Help.");
  Serial.println("");
}

void setup()
{
  Serial.begin(9600); 
  Serial.println("*** Arduino");
  Serial.println("*** DHT3xDog");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  dht1.begin();
  dht2.begin();
  dht3.begin();

  EEPROM_restore();

  showhelp();
}

void reboot()
{
  Serial.println("Rebooting....");
  delay (DELAY_SECOND);
  resetFunc();
}

void reset()
{
  Serial.println("Resetting values.");
  t1max = t2max = t3max = -100;
  t1min = t2min = t3min = 100;
  h1max = h2max = h3max = 0;
  h1min = h2min = h3min = 100;
  EEPROM_save();
}

void loop()
{
  if (Serial.available()) {
    
    char input = Serial.read();  
    if (input != -1) {

      switch (input) {

      case 'h':
        showhelp();
        break;        

      case 'r':
        EEPROM_save();
        reboot();
        break;

      case 'z':
        reset();
        reboot();
        break;
      }
    }
  }
 
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float d1 = t1 - ((100 - h1) / 5);
  if (t1 > t1max) t1max = t1;
  if (t1 < t1min) t1min = t1;
  if (h1 > h1max) h1max = h1;
  if (h1 < h1min) h1min = h1;

  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  float d2 = t2 - ((100 - h2) / 5);
  if (t2 > t2max) t2max = t2;
  if (t2 < t2min) t2min = t2;
  if (h2 > h2max) h2max = h2;
  if (h2 < h2min) h2min = h2;

  float h3 = dht3.readHumidity();
  float t3 = dht3.readTemperature();
  float d3 = t3 - ((100 - h3) / 5);
  if (t3 > t3max) t3max = t3;
  if (t3 < t3min) t3min = t3;
  if (h3 > h3max) h3max = h3;
  if (h3 < h3min) h3min = h3;

  Serial.println(String(++counter) + ".");

  if (isnan(t3) || isnan(h3)) {
    Serial.println("Failed to read from DHT3");
  } else {
    Serial.print("T3: " + fTemp(t3) + "C "); 
    Serial.print("(" + fTemp(t3min) + " - " + fTemp(t3max) + ")   "); 
    Serial.print("H3: " + fHumi(h3) + "% "); 
    Serial.print("(" + fHumi(h3min) + " - " + fHumi(h3max) + ")   "); 
    Serial.println("DP3: " + fTemp(d3) + "C"); 
  }

  if (isnan(t2) || isnan(h2)) {
    Serial.println("Failed to read from DHT2");
  } else {
    Serial.print("T2: " + fTemp(t2) + "C "); 
    Serial.print("(" + fTemp(t2min) + " - " + fTemp(t2max) + ")   "); 
    Serial.print("H2: " + fHumi(h2) + "% "); 
    Serial.print("(" + fHumi(h2min) + " - " + fHumi(h2max) + ")   "); 
    Serial.println("DP2: " + fTemp(d2) + "C"); 
  }

  if (isnan(t1) || isnan(h1)) {
    Serial.println("Failed to read from DHT1");
  } else {
    Serial.print("T1: " + fTemp(t1) + "C "); 
    Serial.print("(" + fTemp(t1min) + " - " + fTemp(t1max) + ")   "); 
    Serial.print("H1: " + fHumi(h1) + "% "); 
    Serial.print("(" + fHumi(h1min) + " - " + fHumi(h1max) + ")   "); 
    Serial.println("DP1: " + fTemp(d1) + "C"); 
  }

  Serial.println("");

  for (int n = 0; n < 100; ++n) {
    if (Serial.available())
      break;
    delay(DELAY_SECOND / 10);
  }

  if (counter == RESET_COUNTER) {
    EEPROM_save();
    reboot();
  }
}

