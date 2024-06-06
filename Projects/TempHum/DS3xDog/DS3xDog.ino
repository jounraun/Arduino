/*
 * DS3xDog
 * (c) 2017 jtr
 * UNO R3
 */

#define PROXIMA

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#ifdef PROXIMA
#define SERIALNO "01"
DeviceAddress Thermometer1 = { 0x28, 0xFF, 0xE4, 0x1E, 0x61, 0x15, 0x03, 0x5F };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0xE1, 0x6C, 0x61, 0x15, 0x03, 0x22 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x55, 0x29, 0x61, 0x15, 0x03, 0x13 };
#endif

#ifdef ANTARES
#define SERIALNO "02"
DeviceAddress Thermometer1 = { 0x28, 0xFF, 0xD1, 0x29, 0x61, 0x15, 0x03, 0xC7 };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0xD9, 0x5E, 0x61, 0x15, 0x03, 0xD7 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x27, 0x99, 0x61, 0x15, 0x03, 0x6C };
#endif

#ifdef ORION
DeviceAddress Thermometer1 = { 0x28, 0x70, 0xFD, 0xDE, 0x06, 0x00, 0x00, 0x09 };
DeviceAddress Thermometer2 = { 0x28, 0x25, 0xE0, 0xDF, 0x06, 0x00, 0x00, 0xD3 };
DeviceAddress Thermometer3 = { 0x28, 0xB5, 0xF0, 0xCA, 0x06, 0x00, 0x00, 0xA7 };
#endif

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** DS3xDog");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");
  
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
  
  sensors.requestTemperatures();
  
  t1 = getTemperature(Thermometer1);
  t2 = getTemperature(Thermometer2);
  t3 = getTemperature(Thermometer3);

  Serial.println(String(++counter) + ". " + String(t1) + " C  " + String(t2) + " C  " + String(t3) + " C");

  for (int n = 1; n <= 10; ++n)
    delay(DELAY_SECOND);

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

