/*
 * APODS
 * (c) 2015 jtr
 * ???
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress Thermometer1 = { 0x28, 0x6E, 0x53, 0xDF, 0x06, 0x00, 0x00, 0x5F };
DeviceAddress Thermometer2 = { 0x28, 0x23, 0x2B, 0xE1, 0x06, 0x00, 0x00, 0x6E };
DeviceAddress Thermometer3 = { 0x28, 0x6B, 0x17, 0xCB, 0x06, 0x00, 0x00, 0x74 };

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** APODS");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");
  
  sensors.begin();
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer2, 10);
  sensors.setResolution(Thermometer3, 10);
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

  Serial.println(String(++counter) + ". " + String(t1)+" C  " + String(t2)+" C  " + String(t3)+" C");

  for (int n = 1; n <= 1; ++n)
    delay(10000);

  if (counter == RESET_COUNTER) {
    reboot();
  }
}

void reboot()
{
  Serial.println("Rebooting....");
  delay(DELAY_SECOND);
  resetFunc();
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
