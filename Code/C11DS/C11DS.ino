// This Arduino sketch reads DS18B20 "1-Wire" digital
// temperature sensors.
// Tutorial:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress Thermometer1 = { 0x28, 0xFF, 0xE4, 0x1E, 0x61, 0x15, 0x03, 0x5F };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0xE1, 0x6C, 0x61, 0x15, 0x03, 0x22 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x55, 0x29, 0x61, 0x15, 0x03, 0x13 };

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** C11DS");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");
  
  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer2, 10);
  sensors.setResolution(Thermometer3, 10);

//  discoverOneWireDevices();
}

float getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00)
    Serial.print("Error getting temperature");
  return (tempC);
}

void loop(void)
{
  float t1, t2, t3;
  
  sensors.requestTemperatures();
  
  t1 = getTemperature(Thermometer1);
  t2 = getTemperature(Thermometer2);
  t3 = getTemperature(Thermometer3);

  Serial.println(String(++counter) + ". " + String(t1) + " " + String(t2) + " " + String(t3));

  for (int n = 1; n <= 1; ++n)
    delay(10000);

  if (counter == RESET_COUNTER) {
     Serial.println("Rebooting....");
     delay(DELAY_SECOND);
     resetFunc();
  }
}

void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  
  Serial.println("Looking for 1-Wire devices...");
  while (oneWire.search(addr)) {
    Serial.println("\nFound 1-Wire device with address:");
    for (i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if (OneWire::crc8( addr, 7) != addr[7]) {
        Serial.println("\nCRC is not valid!");
        return;
    }
  }
  Serial.println("\nThat's it.");
  oneWire.reset_search();
  return;
}



