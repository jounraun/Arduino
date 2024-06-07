/*
 * TelnetBuster
 */

// For W5100
#include <SPI.h>
#include <Ethernet.h>

// For enc28j60
//#include <UIPEthernet.h>

// You must also update FullIP/FullIP.h
#include <FullIP.h>

#include <MemoryFree.h>

#include <EEPROM.h>
#include <DHT.h>

TelnetServer telnet;
unsigned long next;

#define SERIALNO "01"

#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define PWM_CHANNELS 4

#define PWM_MIN 0
#define PWM_MAX 255
#define PWM_STEP 10

//int PWM_pin[PWM_CHANNELS + 1] = {0, 10, 9, 6, 3};
int PWM_pin[PWM_CHANNELS + 1] = {0, 2, 2, 2, 2};

int PWM_val[PWM_CHANNELS + 1];
int PWM_current = 1;

#define VT_PIN A0
#define AT_PIN A1

#define RESET_COUNTER 1000
#define DELAY_SECOND  1000

int counter = 0;

void(* resetFunc) (void) = 0;

void setup() {

  Serial.begin(9600);

  showversion(0);
  
  EEPROM_restore();
  PWM_init();
  dht.begin();

  uint8_t mac[6] = {0x10, 0x11, 0x02, 0x03, 0x04, 0x05};
  IPAddress ip(192, 168, 0, 124);

  Ethernet.begin(mac, ip);

  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  telnet.begin(parse);
  Serial.println("Server started.");
  next = 0;
}

void loop() {
  telnet.maintain();
  if (((signed long)(millis() - next)) > 0) {
    Serial.println("Free Memory: " + String(freeMemory()));
    if (telnet.connected()) {
      Serial.println(F("Client is connected."));
    } else {
      counter++;
      Serial.println("Counter: " + String(counter));
      if (counter == RESET_COUNTER) {
        EEPROM_save();
        reboot();
      }
    }
    next = millis() + 10000;
  }
}

byte parse(char buffer[]) {
  
  int ok = 1;
  
  if (strcmp_P(buffer, PSTR("")) == 0) {
    outprintln(F("Welcome!"));
    showversion(1);
  } else if (strcmp_P(buffer, PSTR("v")) == 0) {
    showversion(1);
    return 1;
  } else if (strcmp_P(buffer, PSTR("h")) == 0) {
    showhelp();
    return 1;
  } else if (strcmp_P(buffer, PSTR("?")) == 0) {
    showhelp();
    return 1;
  } else if (strcmp_P(buffer, PSTR("s")) == 0) {
    showstatus();
    return 1;
  } else if (strcmp_P(buffer, PSTR("1")) == 0) {
    PWM_setcur(1);
  } else if (strcmp_P(buffer, PSTR("2")) == 0) {
    PWM_setcur(2);
  } else if (strcmp_P(buffer, PSTR("3")) == 0) {
    PWM_setcur(3);
  } else if (strcmp_P(buffer, PSTR("4")) == 0) {
    PWM_setcur(4);
  } else if (strcmp_P(buffer, PSTR("r")) == 0) {
    EEPROM_save();
    reboot();
  } else if (strcmp_P(buffer, PSTR("q")) == 0) {
    telnet.disconnect();
    return 1;
  } else if (strcmp_P(buffer, PSTR("+")) == 0) {
    PWM_inc(PWM_current, PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("++")) == 0) {
    PWM_inc(PWM_current, 2 * PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("+++")) == 0) {
    PWM_inc(PWM_current, 3 * PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("-")) == 0) {
    PWM_dec(PWM_current, PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("--")) == 0) {
    PWM_dec(PWM_current, 2 * PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("---")) == 0) {
    PWM_dec(PWM_current, 3 * PWM_STEP);
  } else if (strcmp_P(buffer, PSTR("u")) == 0) {
    PWM_set(PWM_current, PWM_MAX);
  } else if (strcmp_P(buffer, PSTR("d")) == 0) {
    PWM_set(PWM_current, PWM_MIN);
  } else {
    ok = 0;
  }

  if (ok) {
    EEPROM_save();
    showstatus();
    return 1;
  } else {
    return 0;
  }
}

void showstatus()
{
    float voltage = get_voltage();
    float current = get_current(); 
    float watts = voltage * current;

    float t = dht_temperature();
    float h = dht_humidity();

    outprint("\nTemperature = " + String(t) + " C\t ");
    outprintln("Humidity = " + String(h) + " %");

    for (int n = 1; n <= PWM_CHANNELS; ++n) {      
      if (n == PWM_current) outprint("[");
      outprint("P" + String(n));
      if (n  == PWM_current) outprint("]");
      outprint(" = " + String(100 * PWM_get(n) / PWM_MAX) + "%  ");
    }
    outprint("\n\rP (" + String(watts) + " W) = ");
    outprint("U (" + String(voltage) + " V) * ");
    outprintln("I ("+String(current) + " A)\n");
}
  
void PWM_init()
{
  for (int n = 1; n <= PWM_CHANNELS; ++n) {
    pinMode(PWM_pin[n], OUTPUT);
    PWM_set(n, PWM_val[n]);
  }
}

void PWM_set(int n, int val)
{
  PWM_val[n] = val;
  analogWrite(PWM_pin[n], val);
}

void PWM_inc(int n, int val)
{
  PWM_val[n] += val;
  if (PWM_val[n] > PWM_MAX) {
    PWM_val[n] = PWM_MAX;
    outprintln(F("Reached maximum value!"));
  }
  analogWrite(PWM_pin[n], PWM_val[n]);  
}

void PWM_dec(int n, int val)
{
  PWM_val[n] -= val;
  if (PWM_val[n] < PWM_MIN) {
    PWM_val[n] = PWM_MIN;
    outprintln("Reached minimum value!");
  }
  analogWrite(PWM_pin[n], PWM_val[n]);  
}

int PWM_get(int n)
{
  return(PWM_val[n]);
}

void PWM_setcur(int n)
{
  PWM_current = n;
}

float get_voltage()
{
return 13.8;
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
return 0.5;
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
  outprintln("");
  outprintln(F("Commands are: +/-/u/d/1/2/3/4/h/v/r"));
  outprintln("");
}

void reboot()
{
  Serial.println(F("Rebooting...."));
  outprintln(F("Rebooting...."));
  delay(DELAY_SECOND);
  resetFunc();
}

void EEPROM_save()
{
  EEPROM.write(0, PWM_current);
  
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
}

float dht_temperature()
{ 
return 22.3;
  float t = dht.readTemperature();

  if (isnan(t)) {
    outprintln(F("Failed to read temperature from DHT11 sensor!"));
    return 0;
  }
  return (t);
}

float dht_humidity()
{
return 45.0;
  float h = dht.readHumidity();

  if (isnan(h)) {
    outprintln(F("Failed to read humidity from DHT11 sensor!"));
    return 0;
  }
  return (h);
}

void outprintln(String s)
{
  telnet.client.println(s);
}

void outprint(String s)
{
  telnet.client.print(s);
}

void showversion(int mode)
{
  String s2 = F("\n*** Arduino");
  String s3 = F("*** TelnetBuster");
  String s4 = "*** SN #";
  String s5 = SERIALNO;
  String s6 = F("*** (c) 2018 jtr\n");

  if (mode == 0) {
     Serial.println(s2);
     Serial.println(s3);
     Serial.println(s4 + s5);
     Serial.println(s6);
  } else {
     outprintln(s2);
     outprintln(s3);
     outprintln(s4 + s5);
     outprintln(s6);
  }
}
