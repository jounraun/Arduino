/*
 * WeatherDog
 * (c) 2016 jtr
 * Mega
 */
 
// #define MYDEBUG
#define MYVERBOSE

/*---------------------------------------------------------------*/
// ENC28J60 Ethernet module

#include <UIPEthernet.h>

// **** ETHERNET SETTING ****
byte mac[] = { 0x64, 0x44, 0x41, 0x33, 0x32, 0x31 };                                       
IPAddress ip(192, 168, 0, 233);                        
EthernetServer server(80);

/*---------------------------------------------------------------*/
// DHT22 Temperature and humidity module (internal)

#include <DHT.h>

#define DHTINT22PIN 2
DHT dht22int(DHTINT22PIN, DHT22);


/*---------------------------------------------------------------*/
// DHT22 Temperature and humidity module (external)

#include <DHT.h>

#define DHTINT22PIN 3
DHT dht22ext(DHTINT22PIN, DHT22);

/*---------------------------------------------------------------*/
// BMP180 Pressure module (internal)

#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/*---------------------------------------------------------------*/
// Rain sensor module (externel)

const int analogInPin1 = A0;

/*---------------------------------------------------------------*/
// MLX90615 IR module

#include "MLX90615.h"
#include <I2cMaster.h>

byte sda = 22;
byte scl = 23;

SoftI2cMaster i2c(sda, scl);
MLX90615 mlx90615(DEVICE_ADDR, &i2c);

/*---------------------------------------------------------------*/
// DS3231 RTC module (internal)

#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

/*---------------------------------------------------------------*/

#define STR_INT_TEMPERATURE  "INT_TEMPERATURE "
#define STR_INT_HUMIDITY     "INT_HUMIDITY    "

#define STR_EXT_TEMPERATURE  "EXT_TEMPERATURE "
#define STR_EXT_HUMIDITY     "EXT_HUMIDITY    "

#define STR_BMP_PRESSURE     "BMP_PRESSURE    "
#define STR_BMP_TEMPERATURE  "BMP_TEMPERATURE "

#define STR_RAIN_VOLUME      "RAIN_VOLUME     "

#define STR_MLX_AMBIENT      "MLX_AMBIENT     "
#define STR_MLX_OBJECT       "MLX_OBJECT      "

#define STR_RTC_DATE         "RTC_DATE        "
#define STR_RTC_TIME         "RTC_TIME        "

String str01a = "";
String str02a = "";
String str03a = "";
String str04a = "";
String str05a = "";
String str06a = "";
String str07a = "";
String str08a = "";
String str09a = "";
String str10a = "";
String str11a = "";
String str12a = "";
String str13a = "";
String str14a = "";
String str15a = "";
String str16a = "";
String str17a = "";
String str18a = "";
String str19a = "";
String str20a = "";

String str01b = "";
String str02b = "";
String str03b = "";
String str04b = "";
String str05b = "";
String str06b = "";
String str07b = "";
String str08b = "";
String str09b = "";
String str10b = "";
String str11b = "";
String str12b = "";
String str13b = "";
String str14b = "";
String str15b = "";
String str16b = "";
String str17b = "";
String str18b = "";
String str19b = "";
String str20b = "";


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
  myprintln("*** WeatherDog");
  myprintln("*** (c) 2016 jtr");

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.println(Ethernet.localIP());

  myprintln("Wire.begin()");
  Wire.begin();

  // set the initial UTC time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(0, 41, 9, 1, 17, 4, 16);

  myprintln("dht22int.begin()");
  dht22int.begin();

  myprintln("dht22ext.begin()");
  dht22ext.begin();

  myprintln("bmp.begin()");
  bmp.begin();

  myprintln("rain_begin()");
  rain_begin();

  myprintln("DONE");
  
}

void loop() {
  mydebug("loop:");
  
  if (++dspcnt == 100) {
    dspcnt = 0;

    str01a = str02a = str03a = str04a = str05a =
    str06a = str07a = str08a = str09a = str10a =
    str11a = str12a = str13a = str14a = str15a =
    str16a = str17a = str18a = str19a = str20a = "";

    str01b = str02b = str03b = str04b = str05b =
    str06b = str07b = str08b = str09b = str10b =
    str11b = str12b = str13b = str14b = str15b =
    str16b = str17b = str18b = str19b = str20b = "";

    myprintln("---------------------------------------");
    myprintln(String(++rstcnt) + ".");

    displayTime();
    
    dht22int_sensor();
    dht22ext_sensor();
    bmp_sensor();
    rain_sensor();
    mlx_sensor();
 
    if (rstcnt == RESET_COUNTER) {
      myprintln("Rebooting....");
      delay(DELAY_SECOND);
      resetFunc();
    }
  }

  listenForEthernetClients();
  
  delay(DELAY_SECOND / 10);

}

void printRow(EthernetClient *client, String s1, String s2)
{
  client->println("<tr>");
  client->println("<td>");
  client->println(s1);
  client->println("</td>");
  client->println("<td>");
  client->println(s2);
  client->println("</td>");
  client->println("</tr>");
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

          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Weather Dog</title>");
          client.println("<meta http-equiv=\"refresh\" content=\"60\">");
          client.println("</head>");

          client.println("<body>");

          client.println("<table>");

          printRow(&client, str01a, str01b);
          printRow(&client, str02a, str02b);
          printRow(&client, str03a, str03b);
          printRow(&client, str04a, str04b);
          printRow(&client, str05a, str05b);
          printRow(&client, str06a, str06b);
          printRow(&client, str07a, str07b);
          printRow(&client, str08a, str08b);
          printRow(&client, str09a, str09b);
          printRow(&client, str10a, str10b);
          
          client.println("</table>");

          client.println("</body>");
          client.println("</html>");

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

void dht22int_sensor()
{
  mydebug("dht22int_sensor:");
  
  float h = dht22int.readHumidity();
  float t = dht22int.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    myprintln("Failed to read from int DHT22 sensor!");
    return;
  }

  str03b = String(t) + " &deg;" + "C";
  str03a = STR_INT_TEMPERATURE;
  myprintln(str03a + " " + str03b);

  str04b = String(h) + " %";
  str04a = STR_INT_HUMIDITY;
  myprintln(str04a + " " + str04b);
}

void dht22ext_sensor()
{
  mydebug("dht22ext_sensor:");
  
  float h = dht22ext.readHumidity();
  float t = dht22ext.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    myprintln("Failed to read from ext DHT22 sensor!");
    return;
  }

  str05b = String(t) + " &deg;" + "C";
  str05a = STR_EXT_TEMPERATURE;
  myprintln(str05a + " " + str05b);

  str06b = String(h) + " %";
  str06a = STR_EXT_HUMIDITY;
  myprintln(str06a + " " + str06b);
}

void bmp_sensor()
{
  mydebug("bmp_sensor:");

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  int p = event.pressure;
  if (p) {
    str07b = String(p) + " hPa";
    str07a = STR_BMP_PRESSURE;
    myprintln(str07a + " " + str07b);
    float t;
    bmp.getTemperature(&t);
    str08b = String(t) + " &deg;" + "C";;
    str08a = STR_BMP_TEMPERATURE;
    myprintln(str08a + " " + str08b);
  }
}

void mlx_sensor()
{
  mydebug("mlx_sensor:");

  float temperatureObj = mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureAmb = mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE);

  str09b = String(temperatureAmb) + " &deg;" + "C";;
  str09a = STR_MLX_AMBIENT;
  myprintln(str09a + " " + str09b);

  str10b = String(temperatureObj) + " &deg;" + "C";;
  str10a = STR_MLX_OBJECT;
  myprintln(str10a + " " + str10b);
}

void rain_begin()
{
  mydebug("rain_begin:");
  
  pinMode(analogInPin1, INPUT);
}

void rain_sensor()
{  
  mydebug("rain_sensor:");

  int sensorValue = analogRead(analogInPin1);

  str11b = String(1024 - sensorValue);
  str11a = STR_RAIN_VOLUME;
  myprintln(str11a + " " + str11b);
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

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  str01b = String(dayOfMonth) + "." + String(month) + "." + "20" + String(year);
  str01a = STR_RTC_DATE;
  myprintln(str01a + " " + str01b);

  str02b = String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second);
  str02a = STR_RTC_TIME;
  myprintln(str02a + " " + str02b);
}

