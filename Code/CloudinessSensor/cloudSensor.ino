#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include "DHT.h"

#define DHTPIN 9     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

#include <VirtualWire.h>

/*
#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 9
*/

#define ONE_WIRE_BUS 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define DS3231_I2C_ADDRESS 0x68
//#define DS3231_I2C_ADDRESS 0x57

#define MYBUFSIZE 80
char msg[MYBUFSIZE];

#define LEDPIN 11

// Light Sensor
#define LS_ANALOG_SENSOR_PIN  A0
#define LS_DIGITAL_SENSOR_PIN 3

// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  Serial.begin(9600);
  dht.begin();
  mlx.begin();
  sensors.begin();
  Wire.begin();

  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  //  set the initial time here:
  //  DS3231 seconds, minutes, hours, day, date, month, year
  //  setDS3231time(10,25,18,7,25,4,15);

  vw_set_tx_pin(12);          // Sets pin D12 as the TX pin
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(4000);	        // Bits per sec

  Serial.println("Weather Station");
  Serial.println("");
  //  delay(1000);
}

void loop() {
  displayTime();
  ds_sensor();
  dht_sensor();
  mlx_sensor();
  light_sensor();
  bmp_sensor();
  sendmsg("");
  delay(1000);
}

void dht_sensor()
{
  String str;
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  str = "TMP1 " + String(t);
  sendmsg(str);

  str = "HUMD " + String(h);
  sendmsg(str);
}

void mlx_sensor()
{
  String str;

  str = "TMP2 " + String(mlx.readAmbientTempC());
  sendmsg(str);

  str = "IR   " + String(mlx.readObjectTempC());
  sendmsg(str);
}

void ds_sensor() {
  String str;

  sensors.requestTemperatures();
  str = "TMP3 " + String(sensors.getTempCByIndex(0));
  sendmsg(str);
}

void light_sensor()
{
  int  switch_state;
  int  LightAnalogValue;
  String str;

  LightAnalogValue = analogRead(LS_ANALOG_SENSOR_PIN);
  str = "LGHT " + String(LightAnalogValue);
  sendmsg(str);
}

void bmp_sensor()
{
  String str;

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    int p = event.pressure;
    str = "PRES " + String(p);
    sendmsg(str);
  }
}


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime()
{
  String str1;
  String str2;
  String str;
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  str1 = String(dayOfMonth) + "." + String(month) + "." + "20" + String(year);
  str2 = String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second);
  str = "DATE " + str1;
  sendmsg(str);
  str = "TIME " + str2;
  sendmsg(str);
}

void sendmsg(String str)
{
  digitalWrite(LEDPIN, true);  // Flash a light to show sending message
  Serial.println(str);
  str.toCharArray(msg, MYBUFSIZE);
  vw_send((uint8_t *)msg, strlen(msg));  //Sending the message
  vw_wait_tx();                         // Wait until the whole message is gone
  digitalWrite(LEDPIN, false);
  delay(1000);
}
