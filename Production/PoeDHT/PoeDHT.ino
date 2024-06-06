#include <UIPEthernet.h>

#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define DHT1PIN 2    // modify to the pin we connected
#define DHT2PIN 3    // modify to the pin we connected
#define DHT3PIN 4    // modify to the pin we connected

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

/*
  float t1 = dht1.readTemperature();
  float t2 = dht2.readTemperature();
  float t3 = dht3.readTemperature();

  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();
  float h3 = dht3.readHumidity();

  Serial.println("T1 = " + String(t1) + " T2 = " + String(t2) + " T3 = " + String(t3));
  Serial.println("H1 = " + String(h1) + " H2 = " + String(h2) + " H3 = " + String(h3));
  Serial.println("");
  
  delay(5000);
*/

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

client.println(F("<sensor1>"));
#ifdef AURIGA
client.println(F("<name1>Sensor#1</name1>"));
#endif
s = "<temp1>" + String(t1) + "</temp1>";
client.println(s);
s = "<humi1>" + String(h1) + "</humi1>";
client.println(s);
client.println(F("</sensor1>"));

client.println(F("<sensor2>"));
#ifdef AURIGA
client.println(F("<name2>Sensor#2</name2>"));
#endif
s = "<temp2>" + String(t2) + "</temp2>";
client.println(s);
s = "<humi2>" + String(h2) + "</humi2>";
client.println(s);
client.println(F("</sensor2>"));

client.println(F("<sensor3>"));
#ifdef AURIGA
client.println(F("<name3>Sensor#3</name3>"));
#endif
s = "<temp3>" + String(t3) + "</temp3>";
client.println(s);
s = "<humi3>" + String(h3) + "</humi3>";
client.println(s);
client.println(F("</sensor3>"));

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

