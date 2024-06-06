/*
 * MiniWeather
 * (c) 2017 jtr
 * Mega
 */

#include <UIPEthernet.h>

//#define DEBUG

byte mac[] = { 0x74, 0x65, 0x56, 0x57, 0x58, 0x17 };                                       
IPAddress ip(192, 168, 0, 17);                        
EthernetServer server(80);

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
//#define DHTTYPE DHT22   // DHT 22 (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// Initialize DHT sensor for normal 16mhz Arduino
#define DHT11PIN 7
#define DHT21PIN 8
//#define DHT22PIN 9

//DHT dht22(DHT22PIN, DHT22);
DHT dht11(DHT11PIN, DHT11);
DHT dht21(DHT21PIN, DHT21);

#define ONE_WIRE_BUS 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Light Sensor
#define LS_ANALOG_SENSOR_PIN A0

// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// TLS2561
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define STR_S1_TEMPERATURE  "DHT11_TEMPERATURE"
#define STR_S1_HUMIDITY     "DHT11_HUMIDITY"
#define STR_S2_PRESSURE     "BMP_PRESSURE"
#define STR_S2_TEMPERATURE  "BMP_TEMPERATURE"
#define STR_S3_AMBIENT      "MLX_AMBIENT"
#define STR_S3_OBJECT       "MLX_OBJECT"
#define STR_S4_TEMPERATURE  "DHT21_TEMPERATURE"
#define STR_S4_HUMIDITY     "DHT21_HUMIDITY"
#define STR_S5_TEMPERATURE  "DS_TEMPERATURE"
#define STR_S6_LIGHT        "TSL_LIGHTNESS"
#define STR_S7_DARK         "ANA_DARKNESS"

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
  Wire.begin();
  sensors.begin();

Serial.println(2);
  dht11.begin();
  
Serial.println(3);
  dht21.begin();

Serial.println(4);
  if (!bmp.begin()) {
    Serial.print("Ooops, no BMP085 detected.");
  }

Serial.println(5);
  mlx.begin();

Serial.println(6);

  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
  if (!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected.");
  }
Serial.println(7);

Serial.println(99);
  
  delay(DELAY_SECOND);
}

void loop() {

#ifdef DEBUG
  Serial.println("loop:");
#endif

  Serial.println("---------------------------------------");
  Serial.println(++rstcnt);

  dht11_sensor();
  dht21_sensor();
  bmp_sensor();
  mlx_sensor();
  ds_sensor();
  tsl_sensor();
  light_sensor();

  if (rstcnt == RESET_COUNTER) {
     Serial.println("Rebooting....");
     delay(DELAY_SECOND);
     resetFunc();
  }

  for (int n = 1; n <= 1000; ++n) {
    listenForEthernetClients();
    delay(10);
  }

}

void listenForEthernetClients() {
#ifdef DEBUG
//  Serial.println("listenForEthernetClients:");
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

void dht11_sensor()
{
#ifdef DEBUG
  Serial.println("dht11_sensor:");
#endif

  String str;
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }

  str = STR_S1_TEMPERATURE + String(" ") + String(t);
  Serial.println(str);
  str01 = str;

  str = STR_S1_HUMIDITY + String(" ") + String(h);
  Serial.println(str);
  str02 = str;
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
    str = STR_S2_PRESSURE + String(" ") + String(p);
    Serial.println(str);
    str03 = str;

    float temperature;
    bmp.getTemperature(&temperature);
    str = STR_S2_TEMPERATURE + String(" ") + String(temperature);
    Serial.println(str);
    str04 = str;
  }
}

void mlx_sensor()
{
#ifdef DEBUG
  Serial.println("mlx_sensor:");
#endif

  String str;

  str = STR_S3_AMBIENT + String(" ") + String(mlx.readAmbientTempC());
  Serial.println(str);
  str05 = str;

  str = STR_S3_OBJECT + String(" ") + String(mlx.readObjectTempC());
  Serial.println(str);
  str06 = str;
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

  str = STR_S4_TEMPERATURE + String(" ") + String(t);
  Serial.println(str);
  str07 = str;

  str = STR_S4_HUMIDITY + String(" ") + String(h);
  Serial.println(str);
  str08 = str;
}

void ds_sensor() {
#ifdef DEBUG
  Serial.println("ds_sensor:");
#endif

  String str;

  sensors.requestTemperatures();
  str = STR_S5_TEMPERATURE + String(" ") + String(sensors.getTempCByIndex(0));
  Serial.println(str);
  str09 = str;
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
    str = STR_S6_LIGHT + String(" ") + String(event.light);
    Serial.println(str);
    str10 = str;
  }
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


