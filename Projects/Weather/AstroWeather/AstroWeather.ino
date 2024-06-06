/*
 * AstroWeather
 * (c) 2016 jtr
 * Mega
 */

// Ethernet shield
// Ethernet shield attached to pins 10, 11, 12, 13
#include <SPI.h>
#include <Ethernet.h>

// #define DEBUG

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 222);
IPAddress gateway(192, 168, 0, 254);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server(80);

//#include <VirtualWire.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include "DHT.h"

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Initialize DHT sensor for normal 16mhz Arduino
#define DHT22PIN 9
#define DHT21PIN 8
DHT dht22(DHT22PIN, DHT22);
DHT dht21(DHT21PIN, DHT21);

#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define DS3231_I2C_ADDRESS 0x68
//#define DS3231_I2C_ADDRESS 0x57

// Light Sensor
#define LS_ANALOG_SENSOR_PIN A0
//#define LS_DIGITAL_SENSOR_PIN 3

// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// TLS2561
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define STR_S1_TEMPERATURE "DS_TEMPERATURE"
#define STR_S2_TEMPERATURE "DHT22_TEMPERATURE"
#define STR_S2_HUMIDITY    "DHT22_HUMIDITY"
#define STR_S3_TEMPERATURE "DHT21_TEMPERATURE"
#define STR_S3_HUMIDITY    "DHT21_HUMIDITY"
#define STR_S4_AMBIENT     "MLX_AMBIENT"
#define STR_S4_OBJECT      "MLX_OBJECT"
#define STR_S5_PRESSURE    "BMP_PRESSURE"
#define STR_S6_DATE        "RTC_DATE"
#define STR_S6_TIME        "RTC_TIME"
#define STR_S7_DARK        "ANA_DARKNESS"
#define STR_S8_LIGHT       "TSL_LIGHTNESS"

String str01 = "";
String str02 = "";
String str03 = "";
String str04 = "";
String str05 = "";
String str06 = "";
String str07 = "";
String str08 = "";
String str09 = "";
String str10 = "";
String str11 = "";
String str12 = "";

#define RESET_COUNTER 30000
#define DELAY_SECOND  1000

int rstcnt = 0;

void(* resetFunc) (void) = 0;

void setup() {

  Serial.begin(9600);

#ifdef DEBUG
  Serial.println("setup:");
#endif

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

Serial.println(1);
  sensors.begin();
Serial.println(2);
  dht22.begin();
Serial.println(3);
  dht21.begin();
Serial.println(4);
  mlx.begin();
Serial.println(5);

  if (!bmp.begin()) {
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

Serial.println(6);

  //  set the initial time here:
  //  DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(00,10,15,4,18,11,15);

  Wire.begin();

Serial.println(7);

  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
  if (!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  
Serial.println(8);

  delay(DELAY_SECOND);
}

void loop() {

#ifdef DEBUG
  Serial.println("loop:");
#endif

  Serial.println("---------------------------------------");
  Serial.println(++rstcnt);

  displayTime();

  ds_sensor();
  dht22_sensor();
  dht21_sensor();
  mlx_sensor();
  bmp_sensor();
  light_sensor();
  tsl_sensor();

  if (rstcnt == RESET_COUNTER) {
     Serial.println("Rebooting....");
     delay(DELAY_SECOND);
     resetFunc();
  }

  listenForEthernetClients();

  delay(DELAY_SECOND);
}

void listenForEthernetClients() {
#ifdef DEBUG
  Serial.println("listenForEthernetClients:");
#endif

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          client.print(str01);
          client.println("<br/>");
          client.print(str02);
          client.println("<br/>");
          client.print(str03);
          client.println("<br/>");
          client.print(str04);
          client.println("<br/>");
          client.print(str05);
          client.println("<br/>");
          client.print(str06);
          client.println("<br/>");
          client.print(str07);
          client.println("<br/>");
          client.print(str08);
          client.println("<br/>");
          client.print(str09);
          client.println("<br/>");
          client.print(str10);
          client.println("<br/>");
          client.print(str11);
          client.println("<br/>");
          client.print(str12);

          client.println("<br/>");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

void ds_sensor() {
#ifdef DEBUG
  Serial.println("ds_sensor:");
#endif

  String str;

  sensors.requestTemperatures();
  str = STR_S1_TEMPERATURE + String(" ") + String(sensors.getTempCByIndex(0));
  Serial.println(str);
  str01 = str;
}

void dht22_sensor()
{
#ifdef DEBUG
  Serial.println("dht22_sensor:");
#endif

  String str;
  float h = dht22.readHumidity();
  float t = dht22.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }

  str = STR_S2_TEMPERATURE + String(" ") + String(t);
  Serial.println(str);
  str02 = str;

  str = STR_S2_HUMIDITY + String(" ") + String(h);
  Serial.println(str);
  str03 = str;
}

void dht21_sensor()
{
#ifdef DEBUG
  Serial.println("dht21_sensor:");
#endif

  String str;
  float h = dht21.readHumidity();
  float t = dht21.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT21 sensor!");
    return;
  }

  str = STR_S3_TEMPERATURE + String(" ") + String(t);
  Serial.println(str);
  str04 = str;

  str = STR_S3_HUMIDITY + String(" ") + String(h);
  Serial.println(str);
  str05 = str;
}

void mlx_sensor()
{
#ifdef DEBUG
  Serial.println("mlx_sensor:");
#endif

  String str;

  str = STR_S4_AMBIENT + String(" ") + String(mlx.readAmbientTempC());
  Serial.println(str);
  str06 = str;

  str = STR_S4_OBJECT + String(" ") + String(mlx.readObjectTempC());
  Serial.println(str);
  str07 = str;
}

void bmp_sensor()
{
#ifdef DEBUG
  Serial.println("bmp_sensor:");
#endif

  String str;

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  int p = event.pressure;
  if (p) {
    str = STR_S5_PRESSURE + String(" ") + String(p);
    Serial.println(str);
    str08 = str;
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
#ifdef DEBUG
  Serial.println("displayTime:");
#endif

  String str;
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  str = String(dayOfMonth) + "." + String(month) + "." + "20" + String(year);
  str = STR_S6_DATE + String(" ") + str;
  Serial.println(str);
  str09 = str;

  str = String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second);
  str = STR_S6_TIME + String(" ") + str;
  Serial.println(str);
  str10 = str;
}

void light_sensor()
{
#ifdef DEBUG
  Serial.println("light_sensor:");
#endif

  int  switch_state;
  int  LightAnalogValue;
  String str;

  LightAnalogValue = analogRead(LS_ANALOG_SENSOR_PIN);
  str = STR_S7_DARK + String(" ") + String(LightAnalogValue);
  Serial.println(str);
  str11 = str;
}

void tsl_sensor()
{
#ifdef DEBUG
  Serial.println("tsl_sensor:");
#endif

  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);
  String str;

  /* Display the results (light is measured in lux) */
  if (event.light) {
    str = STR_S8_LIGHT + String(" ") + String(event.light);
    Serial.println(str);
    str12 = str;
  }
}

