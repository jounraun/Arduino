#include <UIPEthernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>

//#define ANTARES
//#define PROXIMA
//#define AURIGA
//#define CABINE
//#define OUT1
#define OUT2

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#ifdef ANTARES
#define SERIALNO "01"
DeviceAddress Thermometer1 = { 0x28, 0xAB, 0xD1, 0x00, 0x00, 0x00, 0x80, 0xA5 };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0xC1, 0xCB, 0xA1, 0x15, 0x04, 0x25 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x11, 0x8D, 0xA1, 0x15, 0x04, 0x92 };
#endif

#ifdef PROXIMA
#define SERIALNO "02"
DeviceAddress Thermometer1 = { 0x28, 0x33, 0xD0, 0x00, 0x00, 0x00, 0x80, 0x78 };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0x96, 0xEE, 0xA4, 0x15, 0x03, 0xA6 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x47, 0xC9, 0xA1, 0x15, 0x04, 0x75 };
#endif

#ifdef AURIGA
#define SERIALNO "03"
DeviceAddress Thermometer1 = { 0x28, 0xFF, 0xDD, 0x5C, 0x52, 0x16, 0x04, 0x23 };
DeviceAddress Thermometer2 = { 0x28, 0xFF, 0x8F, 0x1F, 0x47, 0x16, 0x03, 0xA1 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x89, 0xB7, 0xA4, 0x15, 0x03, 0xFE };
#endif

#ifdef CABINET
#define SERIALNO "04"
DeviceAddress Thermometer1 = { 0x28, 0x9D, 0xD1, 0x00, 0x00, 0x00, 0x80, 0xFA };
DeviceAddress Thermometer2 = { 0x28, 0xFB, 0xD1, 0x00, 0x00, 0x00, 0x80, 0x8B };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x95, 0xB8, 0xA4, 0x15, 0x03, 0x39 };
#endif

#ifdef OUT1
#define SERIALNO "05"
DeviceAddress Thermometer1 = { 0x28, 0x88, 0x74, 0x7B, 0x07, 0x00, 0x00, 0x61 };
DeviceAddress Thermometer2 = { 0x28, 0x09, 0x19, 0x84, 0x07, 0x00, 0x00, 0xAA };
DeviceAddress Thermometer3 = { 0x28, 0xB9, 0x21, 0x7C, 0x07, 0x00, 0x00, 0x7A };
#endif

#ifdef OUT2
#define SERIALNO "06"
DeviceAddress Thermometer1 = { 0x28, 0x90, 0xD1, 0x00, 0x00, 0x00, 0x80, 0xB0 };
DeviceAddress Thermometer2 = { 0x28, 0x26, 0xD4, 0x00, 0x00, 0x00, 0x80, 0xD7 };
DeviceAddress Thermometer3 = { 0x28, 0xFF, 0x86, 0x91, 0xA1, 0x15, 0x04, 0x5E };
#endif

#ifdef ANTARES
byte mac[] = { 0x34, 0x45, 0x56, 0x97, 0x77, 0x26 };                                       
IPAddress ip(192, 168, 0, 26);
#endif

#ifdef PROXIMA
byte mac[] = { 0x34, 0x45, 0x56, 0x97, 0x77, 0x27 };                                       
IPAddress ip(192, 168, 0, 27);
#endif

#ifdef AURIGA
byte mac[] = { 0x34, 0x45, 0x56, 0x97, 0x77, 0x28 };                                       
IPAddress ip(192, 168, 0, 28);
#endif

#ifdef CABINET
byte mac[] = { 0x34, 0x45, 0x56, 0x97, 0x77, 0x29 };                                       
IPAddress ip(192, 168, 0, 29);
#endif

#ifdef OUT1
byte mac[] = { 0x14, 0x75, 0x56, 0x97, 0x27, 0x29 };                                       
IPAddress ip(192, 168, 0, 46);
#endif

#ifdef OUT2
byte mac[] = { 0x94, 0x65, 0x36, 0x92, 0x57, 0x39 };                                       
IPAddress ip(192, 168, 0, 47);
#endif

EthernetServer server(80);

float t1, t2, t3;

String s;

void setup() {
  Serial.begin(9600);

//  discoverOneWireDevices();

  sensors.begin();

  sensors.setResolution(Thermometer1, 10);
  sensors.setResolution(Thermometer2, 10);
  sensors.setResolution(Thermometer3, 10);

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

}

float getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00)
    Serial.print("Error getting temperature");
  return (tempC);
}

void loop() {
  
  EthernetClient client = server.available();
  
  if (client) {  

    boolean currentLineIsBlank = true;

    while (client.connected()) {

      if (client.available()) {
        char c = client.read();

        if (c == '\n' && currentLineIsBlank)  {

      sensors.requestTemperatures();
  
      t1 = getTemperature(Thermometer1);
      t2 = getTemperature(Thermometer2);
      t3 = getTemperature(Thermometer3);

client.println(F("<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN'"));
client.println(F("'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"));
client.println(F("<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>"));
client.println(F("<head>"));
client.println(F("<title>Temperature monitor</title>"));
client.println(F("<meta http-equiv='refresh' content='60'>"));
client.println(F("<link rel='stylesheet' type='text/css' href='http://acerlink.ddns.net:7777/EncTemp.css'/>"));
client.println(F("</head>"));
client.println(F("<div id='shadow-one'><div id='shadow-two'><div id='shadow-three'><div id='shadow-four'>"));
client.println(F("<div id='content'>"));
client.println(F("<h1 align='center'>Temperature monitor</h1>"));
client.println("<h2 align='center'>SN: #" + String(SERIALNO) + "</h2><br>");

client.println(F("<div id='display'>"));

client.println(F("<div class='rectangle'>"));
#ifdef ANTARES
client.println(F("<div class='text' id='id1'>USB and Ethernet</div>"));
#endif
#ifdef PROXIMA
client.println(F("<div class='text' id='id1'>Rig, Relay, USB and Eth</div>"));
#endif
#ifdef AURIGA
client.println(F("<div class='text' id='id1'>Computer and Arduinos</div>"));
#endif
#ifdef CABINET
client.println(F("<div class='text' id='id1'>Computers and Monitors</div>"));
#endif
#ifdef OUT1
client.println(F("<div class='text' id='id1'>Sensor 3</div>"));
#endif
#ifdef OUT2
client.println(F("<div class='text' id='id1'>Sensor 3</div>"));
#endif

client.print(F("<div class='temp' id='divT1'>"));
s = String(t3) + " &degC</div>"; client.println(s);
client.println(F("</div>"));
client.println(F("<br>"));

client.println(F("<div class='rectangle'>"));
#ifdef ANTARES
client.println(F("<div class='text' id='id2'>Rigrunner and Relay</div>"));
#endif
#ifdef PROXIMA
client.println(F("<div class='text' id='id2'>Power supplies</div>"));
#endif
#ifdef AURIGA
client.println(F("<div class='text' id='id2'>Rig, Relay and Ethernet</div>"));
#endif
#ifdef CABINET
client.println(F("<div class='text' id='id2'>Telescopes</div>"));
#endif
#ifdef OUT1
client.println(F("<div class='text' id='id2'>Sensor 2</div>"));
#endif
#ifdef OUT2
client.println(F("<div class='text' id='id2'>Sensor 2</div>"));
#endif

client.print(F("<div class='temp' id='divT2'>"));
s = String(t2) + " &degC</div>"; client.println(s);
client.println(F("</div>"));
client.println(F("<br>"));

client.println(F("<div class='rectangle'>"));
#ifdef ANTARES
client.println(F("<div class='text' id='id3'>Power supplies</div>"));
#endif
#ifdef PROXIMA
client.println(F("<div class='text' id='id3'>Floor box</div>"));
#endif
#ifdef AURIGA
client.println(F("<div class='text' id='id3'>Power supplies</div>"));
#endif
#ifdef CABINET
client.println(F("<div class='text' id='id3'>Power supplies</div>"));
#endif
#ifdef OUT1
client.println(F("<div class='text' id='id3'>Sensor 1</div>"));
#endif
#ifdef OUT2
client.println(F("<div class='text' id='id3'>Sensor 1</div>"));
#endif

client.print(F("<div class='temp' id='divT3'>"));
s = String(t1) + " &degC</div>"; client.println(s);
client.println(F("</div>"));
client.println(F("<br>"));

client.println(F("</div>"));
client.println(F("</div>"));

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

