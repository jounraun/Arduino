/*
 * DSBuster
 * (c) 2016 jtr
 * UNO R3
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <EEPROM.h>

#define ONE_WIRE_BUS 8

#define PWM_CHANNELS 4

#define PWM_MIN 0
#define PWM_MAX 255
#define PWM_STEP 10

#define PWM_DEFAULT_4 100
#define PWM_DEFAULT_3 100
#define PWM_DEFAULT_2 100
#define PWM_DEFAULT_1 0

int PWM_pin[PWM_CHANNELS + 1] = {0, 3, 5, 6, 9};
int PWM_val[PWM_CHANNELS + 1];
int PWM_current = 1;

#define PWM_MANUAL 1
#define PWM_AUTO   2
int PWM_mode = PWM_MANUAL;

#define CHECK_MODE 1
#define NOCHECK_MODE 0

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress Thermometer1 = { 0x28, 0xFF, 0xFC, 0x2C, 0x61, 0x15, 0x03, 0x52 };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0x43, 0x2B, 0x61, 0x15, 0x03, 0xF4 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x47, 0x1D, 0x61, 0x15, 0x03, 0xAA };

#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

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
  Serial.println("*** DSBuster");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");

  sensors.begin();
  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer2, 10);
  sensors.setResolution(Thermometer3, 10);

//  discoverOneWireDevices();

  EEPROM_restore();
  PWM_init();
 
  dht.begin();
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

  sensors.requestTemperatures();
  
  t1 = getTemperature(Thermometer1);
  t2 = getTemperature(Thermometer2);
  t3 = getTemperature(Thermometer3);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  float voltage = get_voltage();
  float current = get_current(); 
  float watts = voltage * current;

  Serial.println(String(++counter) + ". ");
  Serial.println("T-int: " + String(t) + " C  " + "T1: " + String(t1) + " C  " + "T2: " + String(t2) + " C  " + "T3: " + String(t3) + " C");

  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("[");
    Serial.print("P" + String(n));
    if (PWM_mode == PWM_MANUAL && n  == PWM_current)
      Serial.print("]");
    Serial.print(" = " + String(100 * PWM_get(n) / 255) + "%  ");
  }
  Serial.println(PWM_mode == PWM_MANUAL ? "(Manual)" : "(Auto)");
  Serial.println("P (" + String(watts) + " W) = " + "U (" + String(voltage) + " V) * " + "I (" + String(current) + " A)");
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
  const int MAX_READINGS = 100;
  float vt_read = 0;

  for (int n = 0; n < MAX_READINGS; ++n) {
     vt_read += analogRead(VT_PIN);
  }

  float voltage = vt_read * (5.0 / 1024.0) * 5.0;
  return (voltage / MAX_READINGS);
} 

float get_current()
{
  const int MAX_READINGS = 100;
  float at_read = 0;
  
  for (int n = 0; n < MAX_READINGS; ++n) {
     at_read += analogRead(AT_PIN);
  }

  float current = at_read * (5.0 / 1024.0);
  return (current / MAX_READINGS);
}  

void showhelp()
{
  Serial.println("Press +/-/u/d/1/2/3/4/m/a/h/r to control");
  Serial.println("the state of the panel.");

  Serial.println("");
  Serial.println("T1 = Tube Temperature");
  Serial.println("T2 = Ambient Temperature");
  Serial.println("");
  
  Serial.println("P1 = nc");
  Serial.println("P2 = Tube Heater");
  Serial.println("P3 = Finder Heater");
  Serial.println("P4 = Mount Heaters");
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

