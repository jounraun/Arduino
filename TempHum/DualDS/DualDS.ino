/*
 * DualDS
 * (c) 2016 jtr
 * Nano
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress Thermometer1 = { 0x28, 0x6F, 0x01, 0xCB, 0x06, 0x00, 0x00, 0x48 };
DeviceAddress Thermometer2 = { 0x28, 0xB6, 0xA4, 0xCB, 0x06, 0x00, 0x00, 0xFB };

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Start");
  
  sensors.begin();
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer2, 10);

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
  float t1, t2;
  
  if (Serial.read() == 'r') {
    sensors.requestTemperatures();
    t1 = getTemperature(Thermometer1);
    t2 = getTemperature(Thermometer2);
    Serial.println(String(t1) + " C  " + String(t2) + " C");
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
