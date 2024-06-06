/*
 * DSTemp
 * (c) 2016-2017 jtr
 * Nano
 * 01 = UNO R3
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define SERIALNO "14"

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** DSTemp");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  sensors.begin();
}

void loop(void)
{
  if (Serial.available()) {
    if (Serial.read() == 'r') {
      sensors.requestTemperatures();
      Serial.println(sensors.getTempCByIndex(0));  
    }
  }
  delay(100);
}
