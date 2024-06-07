/*
 * DS3xTemp
 * (c) 2016 jtr
 * UNO R3
 */

#include <OneWire.h>
#include <DallasTemperature.h>

//#define GALILEO
#define ORION

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#ifdef GALILEO
#define SERIALNO "03"
DeviceAddress Thermometer1 = { 0x28, 0x70, 0xFD, 0xDE, 0x06, 0x00, 0x00, 0x09 };
DeviceAddress Thermometer2 = { 0x28, 0x25, 0xE0, 0xDF, 0x06, 0x00, 0x00, 0xD3 };
DeviceAddress Thermometer3 = { 0x28, 0xB5, 0xF0, 0xCA, 0x06, 0x00, 0x00, 0xA7 };
#endif

#ifdef ORION
#define SERIALNO "02"
DeviceAddress Thermometer1 = { 0x28, 0x6E, 0x53, 0xDF, 0x06, 0x00, 0x00, 0x5F };
DeviceAddress Thermometer2 = { 0x28, 0x23, 0x2B, 0xE1, 0x06, 0x00, 0x00, 0x6E };
DeviceAddress Thermometer3 = { 0x28, 0x6B, 0x17, 0xCB, 0x06, 0x00, 0x00, 0x74 };
#endif

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Start");
  
  sensors.begin();
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

  if (Serial.available()) {
    if (Serial.read() == 'r') {

      sensors.requestTemperatures();
  
      t1 = getTemperature(Thermometer1);
      t2 = getTemperature(Thermometer2);
      t3 = getTemperature(Thermometer3);

      Serial.println("T1 = " + String(t1) + " C  " + "T2 = " + String(t2) + " C  " + "T3 = " + String(t3) + " C");

      delay(100);
    }
  }
}

void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  
  Serial.println("Looking for 1-Wire devices...");
  while (oneWire.search(addr)) {
    Serial.println();
    Serial.println("Found 1-Wire device with address:");
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
  Serial.println();
  Serial.println("That's it.");
  Serial.println();

  oneWire.reset_search();
  return;
}
