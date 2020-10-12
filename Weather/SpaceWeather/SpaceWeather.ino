/*
 * SpaceWeather
 * (c) 2016 jtr
 * Mega
 */
 
// #define MYDEBUG
#define MYVERBOSE

/*---------------------------------------------------------------*/
// ENC28J60 Ethernet module

#include <UIPEthernet.h>

// **** ETHERNET SETTING ****
byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 };                                       
IPAddress ip(192, 168, 0, 234);                        
EthernetServer server(80);

/*---------------------------------------------------------------*/
// MLX90615 IR module

/*
#include "MLX90615.h"
#include <I2cMaster.h>

byte sda_1 = 22;
byte scl_1 = 23;
byte sda_2 = 24;
byte scl_2 = 25;

SoftI2cMaster i2c_1(sda_1, scl_1);
MLX90615 mlx90615_1(DEVICE_ADDR, &i2c_1);

SoftI2cMaster i2c_2(sda_2, scl_2);
MLX90615 mlx90615_2(DEVICE_ADDR, &i2c_2);
*/

/*---------------------------------------------------------------*/
// MLX90614 IR module (external)

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <OneWire.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

/*---------------------------------------------------------------*/
// DHT11 Temperature and humidity module (internal)

#include <DHT.h>

#define DHT11PIN 3
DHT dht11(DHT11PIN, DHT11);

/*---------------------------------------------------------------*/
// DHT22 Temperature and humidity module (internal)

#include <DHT.h>

#define DHTINT22PIN 2
DHT dhtint22(DHTINT22PIN, DHT22);

/*---------------------------------------------------------------*/
// DS3231 RTC module (internal)

#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

/*---------------------------------------------------------------*/
// BMP180 Pressure module (internal)

#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/*---------------------------------------------------------------*/
// DS18B20 Temperature sensor (external)

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

/*---------------------------------------------------------------*/
// DHT22 Temperature and humidity module (external)

#define DHT22PIN 9    // what pin we're connected to
DHT dht22(DHT22PIN, DHT22);

/*---------------------------------------------------------------*/
// TSL2561 Lightness module (external)

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/*---------------------------------------------------------------*/
// TEMT6000 Lightness module (external)

const int analogInPin1 = A0;
const int analogInPin2 = A1;

/*---------------------------------------------------------------*/
// HMC5883L Magnetometer module (external)

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

/*---------------------------------------------------------------*/

#define STR_MLX_AMBIENT       "MLX_AMBIENT     "
#define STR_MLX_OBJECT        "MLX_OBJECT      "

#define STR_INT1_TEMPERATURE  "INT1_TEMPERATURE"
#define STR_INT1_HUMIDITY     "INT1_HUMIDITY   "

#define STR_INT2_TEMPERATURE  "INT2_TEMPERATURE"
#define STR_INT2_HUMIDITY     "INT2_HUMIDITY   "

#define STR_RTC_DATE          "RTC_DATE        "
#define STR_RTC_TIME          "RTC_TIME        "

#define STR_BMP_PRESSURE      "BMP_PRESSURE    "
#define STR_BMP_TEMPERATURE   "BMP_TEMPERATURE "

#define STR_DS_TEMPERATURE    "DS_TEMPERATURE  "

#define STR_DHT_TEMPERATURE   "DHT_TEMPERATURE "
#define STR_DHT_HUMIDITY      "DHT_HUMIDITY    "

#define STR_TSL_LIGHTNESS     "TSL_LIGHTNESS   "

#define STR_TEMT1_LIGHTNESS   "TEMT1_LIGHTNESS "
#define STR_TEMT2_LIGHTNESS   "TEMT2_LIGHTNESS "

#define STR_HMC_X             "HMC_X           "
#define STR_HMC_Y             "HMC_Y           "
#define STR_HMC_Z             "HMC_Z           "

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
String str13 = "";
String str14 = "";
String str15 = "";
String str16 = "";
String str17 = "";
String str18 = "";
String str19 = "";
String str20 = "";

#define RESET_COUNTER 30000
#define DELAY_SECOND  1000

int dspcnt = 0;
int rstcnt = 0;

void(* resetFunc) (void) = 0;

byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}

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

void myprintln(String s)
{
#ifdef MYVERBOSE
  Serial.println(s);
#endif
}

void mydebug(String s)
{
#ifdef MYDEBUG
  Serial.println(s);
#endif
}

void setup() {
  mydebug("setup:");

  Serial.begin(9600);

  myprintln("*** Arduino");
  myprintln("*** SpaceWeather");
  myprintln("*** (c) 2016 jtr");

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.println(Ethernet.localIP());

  myprintln("Wire.begin()");
  Wire.begin();
  
  // set the initial UTC time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(0, 41, 9, 1, 17, 4, 16);

  myprintln("dht11.begin()");
  dht11.begin();

  myprintln("bmp.begin()");
  bmp.begin();
  
  myprintln("sensors.begin()");
  sensors.begin();

  myprintln("mlx.begin()");
  mlx.begin();

  myprintln("tsl_configure()");
  tsl_configure();

  myprintln("tsl.begin()");
  tsl.begin();

  myprintln("temt_configure()");
  temt_configure();

  myprintln("mag.begin()");
  mag.begin();

  myprintln("DONE");
  
}

void loop() {
  mydebug("loop:");
  
  if (++dspcnt == 100) {
    dspcnt = 0;

    str01 = str02 = str03 = str04 = str05 =
    str06 = str07 = str08 = str09 = str10 =
    str11 = str12 = str13 = str14 = str15 =
    str16 = str17 = str18 = str19 = str20 = "";

    myprintln("---------------------------------------");
    myprintln(String(++rstcnt) + ".");

    displayTime();

    mlx_sensor();
    bmp_sensor();
    dht11_sensor();
    dhtint22_sensor();
    ds_sensor();
    dht22_sensor();
    tsl_sensor();
    temt_sensor();
    mag_sensor();

    if (rstcnt == RESET_COUNTER) {
      myprintln("Rebooting....");
      delay(DELAY_SECOND);
      resetFunc();
    }
  }

  listenForEthernetClients();
  
  delay(DELAY_SECOND / 10);

}

void listenForEthernetClients() {
  mydebug("listenForEthernetClients:");

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    myprintln("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          client.print(str01);
          client.println("<br>");
          client.print(str02);
          client.println("<br>");
          client.print(str03);
          client.println("<br>");
          client.print(str04);
          client.println("<br>");
          client.print(str05);
          client.println("<br>");
          client.print(str06);
          client.println("<br>");
          client.print(str07);
          client.println("<br>");
          client.print(str08);
          client.println("<br>");
          client.print(str09);
          client.println("<br>");
          client.print(str10);
          client.println("<br>");
          client.print(str11);
          client.println("<br>");
          client.print(str12);
          client.println("<br>");
          client.print(str13);
          client.println("<br>");
          client.print(str14);
          client.println("<br>");
          client.print(str15);
          client.println("<br>");
          client.print(str16);
          client.println("<br>");
          client.print(str17);
          client.println("<br>");
          client.print(str18);
          client.println("<br>");
          client.print(str19);
          client.println("<br>");
          client.print(str20);
          client.println("<br>");

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

/*
void mlx_sensor()
{
  mydebug("mlx_sensor:");

  String str;

  float temperatureObj1 = mlx90615_1.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureObj2 = mlx90615_2.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureAmb1 = mlx90615_1.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
  float temperatureAmb2 = mlx90615_2.getTemperature(MLX90615_AMBIENT_TEMPERATURE); 

  str = STR_MLX1_AMBIENT + String(" ") + String(temperatureAmb1);
  myprintln(str);
  str01 = str;

  str = STR_MLX1_OBJECT + String(" ") + String(temperatureObj1);
  myprintln(str);
  str02 = str;

  str = STR_MLX2_AMBIENT + String(" ") + String(temperatureAmb2);
  myprintln(str);
  str03 = str;

  str = STR_MLX2_OBJECT + String(" ") + String(temperatureObj2);
  myprintln(str);
  str04 = str;

}
*/

void mlx_sensor()
{
  mydebug("mlx_sensor:");

  String str;

  float temperatureObj = mlx.readObjectTempC();
  float temperatureAmb = mlx.readAmbientTempC();

  str = STR_MLX_AMBIENT + String(" ") + String(temperatureAmb);
  myprintln(str);
  str01 = str;

  str = STR_MLX_OBJECT + String(" ") + String(temperatureObj);
  myprintln(str);
  str02 = str;

}

void dht11_sensor()
{
  mydebug("dht11_sensor:");

  String str;
  
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    myprintln("Failed to read from DHT11 sensor!");
    return;
  }

  str = STR_INT1_TEMPERATURE + String(" ") + String(t);
  myprintln(str);
  str04 = str;

  str = STR_INT1_HUMIDITY + String(" ") + String(h);
  myprintln(str);
  str05 = str;
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  mydebug("readDS3231time:");

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
  mydebug("displayTime:");

  String str;
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  str = String(dayOfMonth) + "." + String(month) + "." + "20" + String(year);
  str = STR_RTC_DATE + String(" ") + str;
  myprintln(str);
  str06 = str;

  str = String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second);
  str = STR_RTC_TIME + String(" ") + str;
  myprintln(str);
  str07 = str;
}

void bmp_sensor()
{
  mydebug("bmp_sensor:");

  String str;

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  int p = event.pressure;
  if (p) {
    str = STR_BMP_PRESSURE + String(" ") + String(p);
    myprintln(str);
    str08 = str;
    float t;
    bmp.getTemperature(&t);
    str = STR_BMP_TEMPERATURE + String(" ") + String(t);
    myprintln(str);
    str09 = str;
    
  }
}

void dhtint22_sensor()
{
  mydebug("dhtint22_sensor:");

  String str;
  
  float h = dhtint22.readHumidity();
  float t = dhtint22.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    myprintln("Failed to read from int DHT22 sensor!");
    return;
  }

  str = STR_INT2_TEMPERATURE + String(" ") + String(t);
  myprintln(str);
  str10 = str;

  str = STR_INT2_HUMIDITY + String(" ") + String(h);
  myprintln(str);
  str11 = str;
}

void ds_sensor()
{
  mydebug("ds_sensor:");

  String str;
  float t;
  
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);  

  str = STR_DS_TEMPERATURE + String(" ") + String(t);
  myprintln(str);
  str12 = str;
}

void dht22_sensor()
{
  mydebug("dht22_sensor:");

  String str;
  
  float h = dht22.readHumidity();
  float t = dht22.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    myprintln("Failed to read from ext DHT22 sensor!");
    return;
  }

  str = STR_DHT_TEMPERATURE + String(" ") + String(t);
  myprintln(str);
  str13 = str;

  str = STR_DHT_HUMIDITY + String(" ") + String(h);
  myprintln(str);
  str14 = str;
}

void tsl_configure()
{
    /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
 // Serial.println("------------------------------------");
 // Serial.print  ("Gain:         "); Serial.println("Auto");
 // Serial.print  ("Timing:       "); Serial.println("13 ms");
 // Serial.println("------------------------------------");
}

void tsl_sensor()
{  
  mydebug("tsl_sensor:");

  String str;

  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light) {
    str = STR_TSL_LIGHTNESS + String(" ") + String(event.light);
  } else {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
//    str = "Sensor overload";
    str = STR_TSL_LIGHTNESS + String(" 0");
  }
  
  myprintln(str);
  str03 = str;
}

void temt_configure()
{
  pinMode(analogInPin1, INPUT);
  pinMode(analogInPin2, INPUT);
}

void temt_sensor()
{  
  mydebug("temt_sensor:");

  String str;

  int sensorValue1 = analogRead(analogInPin1);
  int sensorValue2 = analogRead(analogInPin2);

  str = STR_TEMT1_LIGHTNESS + String(" ") + String(sensorValue1);
  myprintln(str);
  str15 = str;

  str = STR_TEMT2_LIGHTNESS + String(" ") + String(sensorValue2);
  myprintln(str);
  str16 = str;
}

void mag_sensor()
{  
  mydebug("mag_sensor:");

  String str;

  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);

  str = STR_HMC_X + String(" ") + String(event.magnetic.x);
  myprintln(str);
  str17 = str;

  str = STR_HMC_Y + String(" ") + String(event.magnetic.y);
  myprintln(str);
  str18 = str;

  str = STR_HMC_Z + String(" ") + String(event.magnetic.z);
  myprintln(str);
  str19 = str;
}
