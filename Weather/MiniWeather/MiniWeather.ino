/*
 * MiniWeather
 * (c) 2016 jtr
 * Nano
 */

#include <UIPEthernet.h>

#define SERIALNO 1

#if SERIALNO == 1
byte mac[] = { 0x01, 0x03, 0x38, 0x39, 0x38, 0x31 };                                       
IPAddress ip(192, 168, 0, 161);                        
#endif

#if SERIALNO == 2
byte mac[] = { 0x01, 0x03, 0x38, 0x39, 0x38, 0x32 };                                       
IPAddress ip(192, 168, 0, 162);                        
#endif

EthernetServer server(80);

#include <DHT.h>

#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  dht.begin();
  bmp.begin();

}

void loop() {
  EthernetClient client = server.available();

  if (client) {  
    boolean currentLineIsBlank = true;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {

          float h = dht.readHumidity();
          float t = dht.readTemperature();
          
          sensors_event_t event;
          bmp.getEvent(&event);
          float p = event.pressure;

          String s = String(t) + " " + String(h) + " " + String(p);
          client.println("<html><body>" + s + "</body>");
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
