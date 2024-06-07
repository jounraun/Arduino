/*
 * DSDog
 * (c) 2016 jtr
 * ???
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#define RESET_COUNTER 1440
#define DELAY_SECOND  1000

int rstcnt = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** DSDog");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");

  sensors.begin();
}

void loop(void)
{ 
  sensors.requestTemperatures();
  Serial.println(String(++rstcnt) + ". " + String(sensors.getTempCByIndex(0)) + " C");  

  if (rstcnt == RESET_COUNTER) {
    Serial.println("Rebooting....");
    delay(DELAY_SECOND);
    resetFunc();
  }

  for (int n = 1; n <= 60; ++n)
    delay(DELAY_SECOND);
}
