/*
 * DHT1xDog
 * (c) 2017 jtr
 * UNO R3 (Auriga)
 * Nano (Skynet)
 */

#include "DHT.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

/* Auriga */
#define SERIALNO "01"

#define DHT1PIN 4    // modify to the pin we connected

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht1(DHT1PIN, DHTTYPE);

#define RESET_COUNTER 1000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

float t1max;
float t1min;
float h1max;
float h1min;

void EEPROM_save()
{
  int eeAddress = 0;

  EEPROM.put(eeAddress, t1max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, t1min);
  eeAddress += sizeof(float);

  EEPROM.put(eeAddress, h1max);
  eeAddress += sizeof(float);
  
  EEPROM.put(eeAddress, h1min);
  eeAddress += sizeof(float);

}

void EEPROM_restore()
{ 
  int eeAddress = 0;
  
  EEPROM.get(eeAddress, t1max);
  eeAddress += sizeof(float);
    
  EEPROM.get(eeAddress, t1min);
  eeAddress += sizeof(float);

  EEPROM.get(eeAddress, h1max);
  eeAddress += sizeof(float);
    
  EEPROM.get(eeAddress, h1min);
  eeAddress += sizeof(float);

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
  Serial.println("*** DHT1xDog");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  dht1.begin();
  
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
  t1max = -100;
  t1min = 100;
  h1max = 0;
  h1min = 100;
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

  Serial.println(String(++counter) + ".");

  // check if returns are valid, if they are NaN (not a number) then something went wrong!

  if (isnan(t1) || isnan(h1)) {
    Serial.println("Failed to read from DHT1");
  } else {
    Serial.print("T: " + String(t1) + "C "); 
    Serial.print("(" + String(t1min) + " - " + String(t1max) + ")   "); 
    Serial.print("H: " + String(h1) + "% "); 
    Serial.print("(" + String(h1min) + " - " + String(h1max) + ")   "); 
    Serial.println("DP: " + String(d1) + "C"); 
  }
  
  Serial.println("");

  for (int n = 1; n <= 100; ++n) {
    if (Serial.available())
      break;
    delay(DELAY_SECOND / 10);
  }

  if (counter == RESET_COUNTER) {
    EEPROM_save();
    reboot();
  }
}

