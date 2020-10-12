/*
 * Multimeter
 * (c) 2016 jtr
 * Nano
 */

#define SERIALNO "01"

#define MAXREADINGS 10

#define RESET_COUNTER 30000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

void setup()
{
  Serial.begin(9600);

  Serial.println("*** Arduino");
  Serial.println("*** Multimeter");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2016 jtr");
  Serial.println("");

}

float readVolts()
{
  float volts = 0.0;

  for (int n = 1; n <= MAXREADINGS; ++n) {
    volts = volts + analogRead(A0) * (24.0 / 1024);
    delay(100);
  }
//  Serial.println(analogRead(A0));

  return (volts / MAXREADINGS);
}

float readAmps()
{
  float amps = 0.0;

  for (int n = 1; n <= MAXREADINGS; ++n) {
    amps = amps + (analogRead(A1) - 508) * (30.0 / 512);
    delay(20);
  }
//  Serial.println(analogRead(A1));
  
  return (amps / MAXREADINGS);
}

void loop()
{
  float volts;
  float amps;

  Serial.println(String(++counter) + ".");

  volts = readVolts();
  amps = readAmps();

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
