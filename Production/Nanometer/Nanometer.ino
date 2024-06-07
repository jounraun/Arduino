/*
 * Nanometer
 * (c) 2016 jtr
 * Nano
 */

#define SERIALNO "02"

#define VT_PIN A0
#define AT_PIN A1

#define MAXREADINGS 10

#define RESET_COUNTER 30000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

void setup()
{
  Serial.begin(9600);

  Serial.println("*** Arduino");
  Serial.println("*** Nanometer");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2016 jtr");
  Serial.println("");

}

float get_voltage()
{
  const int MAX_READINGS = 10;
  float vt_read = 0;

  for (int n = 0; n < MAX_READINGS; ++n) {
     vt_read += analogRead(VT_PIN);
     delay(100);
  }

  float voltage = vt_read * (5.0 / 1024.0) * 5.0;
  return (voltage / MAX_READINGS);
} 

float get_current()
{
  const int MAX_READINGS = 10;
  float at_read = 0;
  
  for (int n = 0; n < MAX_READINGS; ++n) {
     at_read += analogRead(AT_PIN);
     delay(100);
  }

  float current = at_read * (5.0 / 1024.0);
  return(current / MAX_READINGS);
}  

void loop()
{
  float volts;
  float amps;

  Serial.println(String(++counter) + ".");

  volts = get_voltage();
  amps = get_current();

  Serial.println(String(volts) + " V\t" + String(amps) + " A\n");
  
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
