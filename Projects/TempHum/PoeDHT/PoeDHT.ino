#include <UIPEthernet.h>

#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define DHT1PIN 5    // modify to the pin we connected
#define DHT2PIN 6    // modify to the pin we connected
#define DHT3PIN 7    // modify to the pin we connected

//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);
DHT dht3(DHT3PIN, DHTTYPE);

#define AURIGA

#ifdef AURIGA
#define SERIALNO "01"
byte mac[] = { 0x14, 0x15, 0x96, 0x87, 0x47, 0x28 };                                       
IPAddress ip(192, 168, 0, 12);
#endif

EthernetServer server(80);

float t1, t2, t3;
float h1, h2, h3;

String s;

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  dht1.begin();
  dht2.begin();
  dht3.begin();

}

void loop() {
  
  EthernetClient client = server.available();
  
  if (client) {  

    boolean currentLineIsBlank = true;

    while (client.connected()) {

      if (client.available()) {
        char c = client.read();

        if (c == '\n' && currentLineIsBlank)  {

          float t1 = dht1.readTemperature();
          float t2 = dht2.readTemperature();
          float t3 = dht3.readTemperature();

          float h1 = dht1.readHumidity();
          float h2 = dht2.readHumidity();
          float h3 = dht3.readHumidity();

client.println(F("<?xml version='1.0' encoding='UTF-8'?>"));

client.println(F("<response>"));

client.println(F("<sensor>"));
#ifdef AURIGA
client.println(F("<name>Computer and Arduinos</name>"));
#endif
s = "<temp>" + String(t1) + "</temp>";
client.println(s);
s = "<humi>" + String(h1) + "</humi>";
client.println(s);
client.println(F("</sensor>"));

client.println(F("<sensor>"));
#ifdef AURIGA
client.println(F("<name>Rig, Relay and Ethernet</name>"));
#endif
s = "<temp>" + String(t2) + "</temp>";
client.println(s);
s = "<humi>" + String(h2) + "</humi>";
client.println(s);
client.println(F("</sensor>"));

client.println(F("<sensor>"));
#ifdef AURIGA
client.println(F("<name>Power supplies</name>"));
#endif
s = "<temp>" + String(t3) + "</temp>";
client.println(s);
s = "<humi>" + String(h3) + "</humi>";
client.println(s);
client.println(F("</sensor>"));

client.println(F("</response>"));

          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      } 
    }

    delay(10);
    client.stop();
  }
}

