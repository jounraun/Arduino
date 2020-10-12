/*
 * FETBuster
 * (c) 2017 jtr
 * Nano
 */

#include <EEPROM.h>
#include <DHT.h>

#define SERIALNO "13"

#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define PWM_CHANNELS 4

#define PWM_MIN 0
#define PWM_MAX 255
#define PWM_STEP 10

int PWM_pin[PWM_CHANNELS + 1] = {0, 10, 9, 6, 3};
int PWM_val[PWM_CHANNELS + 1];
int PWM_current = 1;

#define PWM_MANUAL 1
#define PWM_AUTO   2
int PWM_mode = PWM_MANUAL;

#define CHECK_MODE 1
#define NOCHECK_MODE 0

#define VT_PIN A0
#define AT_PIN A1

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** FETBuster");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  EEPROM_restore();
  PWM_init();
  dht.begin();
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

      default:
        if (input != '\r' && input != '\n')
          Serial.println("Invalid entry!");
        break;
      }
      EEPROM_save();
    }
  }
  
  float voltage = get_voltage();
  float current = get_current(); 
  float watts = voltage * current;

  Serial.println(String(++counter) + ". ");

  t = dht_temperature();
  h = dht_humidity();

  Serial.println("Temperature = " + String(t) + " C\t" + " " + "Humidity = " + String(h) + " %");

  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("[");
    Serial.print("P" + String(n));
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("]");
    Serial.print(" = " + String(100 * PWM_get(n) / PWM_MAX) + "%  ");
  }
  Serial.println(PWM_mode == PWM_MANUAL ? "(Manual)" : "(Auto)");
  Serial.println("P (" + String(watts) + " W) = " + "U (" + String(voltage) + " V) * " + "I ("+String(current) + " A)");
  Serial.println();

/*
  if (PWM_mode == PWM_AUTO) {
    int pwm = PWM_MIN;
    if (t < 30) pwm = 10 / 100.0 * PWM_MAX;
    if (t < 25) pwm = 20 / 100.0 * PWM_MAX;
    if (t < 20) pwm = 40 / 100.0 * PWM_MAX;
    if (t < 15) pwm = 60 / 100.0 * PWM_MAX;
    if (t < 10) pwm = 80 / 100.0 * PWM_MAX;
    if (t < 5) pwm = PWM_MAX;
    for (int n = 1; n <= PWM_CHANNELS; ++n)
      PWM_set(n, pwm, NOCHECK_MODE);
  }
*/
  
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

void mode_error()
{
  Serial.println("Mode error!");  
}

float get_voltage()
{
  const int MAX_READINGS = 10;
  float vt_read = 0;

  for (int n = 0; n < MAX_READINGS; ++n) {
     vt_read += analogRead(VT_PIN);
     delay(10);
  }

  float voltage = vt_read * (5.0 / 1024.0) * 5.0;
  return (voltage / MAX_READINGS);
} 

float get_current()
{
  const int MAX_READINGS = 30;
  float at_read = 0;
  
  for (int n = 0; n < MAX_READINGS; ++n) {
     at_read += analogRead(AT_PIN);
     delay(10);
  }

  float current = at_read * (5.0 / 1024.0);
  return(current / MAX_READINGS);
}  

void showhelp()
{
  Serial.println("Press +/-/u/d/1/2/3/4/m/a/h/r to control");
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
}

void EEPROM_restore()
{ 
  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    PWM_val[n] = EEPROM.read(10 + n);
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

