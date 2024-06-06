/*
 * HeatBuster
 * (c) 2016 jtr
 * Nano
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#include <EEPROM.h>
#include <DHT.h>

#define SERIALNO "01"

#define DHTPIN 4    // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define PWM_CHANNELS 4

#define PWM_MIN 0
#define PWM_MAX 255
#define PWM_STEP 10

#define PWM_DEFAULT_4 100
#define PWM_DEFAULT_3 100
#define PWM_DEFAULT_2 100
#define PWM_DEFAULT_1 0

int PWM_pin[PWM_CHANNELS + 1] = {0, 10, 9, 6, 3};
int PWM_val[PWM_CHANNELS + 1];
int PWM_exc[PWM_CHANNELS + 1] = {0, 0, 0, 0, 0};
int PWM_current = 1;

#define PWM_MANUAL 1
#define PWM_AUTO   2
int PWM_mode = PWM_MANUAL;

#define CHECK_MODE 1
#define NOCHECK_MODE 0

#define PWM_EXCLUDED 1
#define PWM_INCLUDED 0

#define RESET_COUNTER 1000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

float readVolts()
{
  const int MAX_READINGS = 10;
  float volts = 0.0;

  for (int n = 1; n <= MAX_READINGS; ++n) {
    volts = volts + analogRead(A0) * (22.8 / 1024);
    delay(10);
  }

  return (volts / MAX_READINGS);
}

float readAmps()
{
  const int MAX_READINGS = 100;
  float amps = 0.0;

  for (int n = 1; n <= MAX_READINGS; ++n) {
    amps = amps + (analogRead(A1) - 506) * 20.0 / 512;
    delay(1);
  }

//  Serial.println("A1=" + String(analogRead(A1)));
  return (amps / MAX_READINGS);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** HeatBuster");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2016 jtr");
  Serial.println("");

  EEPROM_restore();
  PWM_init();
  dht.begin();
  sensors.begin();
}

void loop(void)
{
  float t1, t2, t3;
  float t, h;

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

      case 'm':
        PWM_mode = PWM_MANUAL;
        break;

      case 'a':
        PWM_mode = PWM_AUTO;
        break;

      case '1':
          PWM_setcur(1);
        break;
      case '2':
          PWM_setcur(2);
        break;
      case '3':
          PWM_setcur(3);
        break;
      case '4':
          PWM_setcur(4);
        break;
        
      case '+':
        PWM_inc(PWM_current, PWM_STEP, CHECK_MODE);
        break;        

      case '-':
        PWM_dec(PWM_current, PWM_STEP, CHECK_MODE);
        break;        

      case 'u':
        PWM_set(PWM_current, PWM_MAX, CHECK_MODE);
        break;        

      case 'd':
        PWM_set(PWM_current, PWM_MIN, CHECK_MODE);
        break;        

      case 'x':
        PWM_exclude(PWM_current);
        break;        

      case 'i':
        PWM_include(PWM_current);
        break;        

      default:
        if (input != '\r' && input != '\n')
          Serial.println("Invalid entry!");
        break;
      }
      EEPROM_save();
    }
  }
  
  float voltage = readVolts();
  float current = readAmps(); 
  float watts = voltage * current;

  sensors.requestTemperatures();
  float t_ext = sensors.getTempCByIndex(0);

  Serial.println(String(++counter) + ". ");

  t = dht_temperature();
  h = dht_humidity();

  Serial.println("Temperature = " + String(t_ext) + " C" + "   " + "T = " + String(t) + " C" + "   " + "H = " + String(h) + " %");

  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("[");
    if (PWM_exc[n] == PWM_INCLUDED)
      Serial.print("*");
    Serial.print("P" + String(n));
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("]");
    Serial.print(" = " + String(100 * PWM_get(n) / PWM_MAX) + "%  ");
  }
  Serial.println(PWM_mode == PWM_MANUAL ? "(Manual)" : "(* = Auto)");
  Serial.println("P (" + String(watts) + " W) = " + "U (" + String(voltage) + " V) * " + "I (" + String(current) + " A)");
  Serial.println();

  if (PWM_mode == PWM_AUTO) {
    int pwm = PWM_MIN;
    if (t_ext < 30) pwm = 10 / 100.0 * PWM_MAX;
    if (t_ext < 25) pwm = 20 / 100.0 * PWM_MAX;
    if (t_ext < 20) pwm = 40 / 100.0 * PWM_MAX;
    if (t_ext < 15) pwm = 60 / 100.0 * PWM_MAX;
    if (t_ext < 10) pwm = 80 / 100.0 * PWM_MAX;
    if (t_ext < 5) pwm = PWM_MAX;
    for (int n = 1; n <= PWM_CHANNELS - 1; ++n) {
      if (PWM_exc[n] == PWM_INCLUDED)
         PWM_set(n, pwm, NOCHECK_MODE);
    }
  }
  
  for (int n = 0; n < 1000; ++n) {
    if (Serial.available())
      break;
    delay(DELAY_SECOND / 100);
  }

  if (counter == RESET_COUNTER) {
     EEPROM_save();
     reboot();
  }
}

void PWM_init()
{
  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    pinMode(PWM_pin[n], OUTPUT);
    PWM_set(n, PWM_val[n], NOCHECK_MODE);
  }
}

void PWM_set(int n, int val, int checkmode)
{
  if (checkmode && PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_val[n] = val;
  analogWrite(PWM_pin[n], val);
}

void PWM_inc(int n, int val, int checkmode)
{
  if (checkmode && PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_val[n] += val;
  if (PWM_val[n] > PWM_MAX) {
    PWM_val[n] = PWM_MAX;    
    Serial.println("Reached maximum value!");
  }
  analogWrite(PWM_pin[n], PWM_val[n]);  
}

void PWM_dec(int n, int val, int checkmode)
{
  if (checkmode && PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_val[n] -= val;
  if (PWM_val[n] < PWM_MIN) {
    PWM_val[n] = PWM_MIN;
    Serial.println("Reached minimum value!");
  }
  analogWrite(PWM_pin[n], PWM_val[n]);  
}

int PWM_get(int n)
{
  return(PWM_val[n]);
}

void PWM_setcur(int n)
{
  if (PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_current = n;
}

void PWM_exclude(int n)
{
  if (PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_exc[n] = PWM_EXCLUDED;
}

void PWM_include(int n)
{
  if (PWM_mode == PWM_AUTO) {
     mode_error();
     return;
  }

  PWM_exc[n] = PWM_INCLUDED;
}

void mode_error()
{
  Serial.println("Mode error!");  
}

void showhelp()
{
  Serial.println("Press +/-/u/d/1/2/3/4/m/a/x/i/h/r to control");
  Serial.println("the state of the panel.");
  Serial.println("");
}

void reboot()
{
  Serial.println("Rebooting....");
  delay(DELAY_SECOND);
  resetFunc();
}

void EEPROM_save()
{
  EEPROM.write(0, PWM_current);
  EEPROM.write(1, PWM_mode);
  
  for (int n = 1; n <= PWM_CHANNELS; ++n)
    EEPROM.write(10 + n, PWM_val[n]);

  for (int n = 1; n <= PWM_CHANNELS; ++n)
    EEPROM.write(20 + n, PWM_exc[n]);
}

void EEPROM_restore()
{ 
  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    PWM_val[n] = EEPROM.read(10 + n);
  }

  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    PWM_exc[n] = EEPROM.read(20 + n);
  }
  
  PWM_current = EEPROM.read(0);
  if (PWM_current == PWM_MAX || PWM_current == 0) 
     PWM_current = 1;

  PWM_mode = EEPROM.read(1);
  if (PWM_mode == PWM_MAX || PWM_mode == 0) 
     PWM_mode = PWM_MANUAL;
}

float dht_temperature()
{
  float t = dht.readTemperature();
  
  if (isnan(t)) {
    Serial.println("Failed to read temperature from DHT11 sensor!");
    return 0;
  }
  return (t);
}

float dht_humidity()
{
  float h = dht.readHumidity();

  if (isnan(h)) {
    Serial.println("Failed to read humidity from DHT11 sensor!");
    return 0;
  }
  return (h);
}

