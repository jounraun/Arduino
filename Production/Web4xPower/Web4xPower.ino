
/*
 * Web4xPower
 * (c) 2016 jtr 
 * UNO
 */

/*---------------------------------------------------------------*/
// ENC28J60 Ethernet module

#include <UIPEthernet.h>

// **** ETHERNET SETTING ****
byte mac[] = { 0x64, 0x64, 0x72, 0x69, 0x34, 0x62 };                                       
IPAddress ip(192, 168, 0, 163);                        
EthernetServer server(80);

/*---------------------------------------------------------------*/

#define RELAY_MIN 1
#define RELAY_MAX 4

#define RELAY_1   1
#define RELAY_2   2
#define RELAY_3   3
#define RELAY_4   4

#define RELAY_OFF 0
#define RELAY_ON  1

int relaystate[RELAY_MAX + 1];
int relaypin[RELAY_MAX + 1] = {0, 7, 6, 5, 4};

String readString;

void SetRelay(int n, int state)
{
  relaystate[n] = state;
  digitalWrite(relaypin[n], state);
}

void setup() {
  
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();

//  Serial.println(Ethernet.localIP());

  for (int n = RELAY_MIN; n <= RELAY_MAX; ++n) {
    pinMode(relaypin[n], OUTPUT);
    SetRelay(n, RELAY_OFF);
  }
}

void loop() {
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
        }

        //if HTTP request has ended
        if (c == '\n') {          
//          Serial.println(readString); //print to serial monitor for debuging

          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println();     
          client.println(F("<HTML>"));
          client.println(F("<HEAD>"));
          client.println(F("<link rel='stylesheet' type='text/css' href='http://acerlink.ddns.net:2344/Web4xPower.css'/>"));
          client.println(F("<TITLE>Web4xPower</TITLE>"));
          client.println(F("</HEAD>"));
          client.println(F("<BODY>"));
          client.println(F("<H1>Web4xPower</H1>"));
          client.println(F("<hr/>"));
          client.println(F("<br>"));
          client.println(F("<a href=\"/?r1on\"\">1 ON</a>"));
          client.println(F("<a href=\"/?r1off\"\">1 OFF</a><br><br><br>"));
          client.println(F("<a href=\"/?r2on\"\">2 ON</a>"));
          client.println(F("<a href=\"/?r2off\"\">2 OFF</a><br><br><br>"));
          client.println(F("<a href=\"/?r3on\"\">3 ON</a>"));
          client.println(F("<a href=\"/?r3off\"\">3 OFF</a><br><br><br>"));
          client.println(F("<a href=\"/?r4on\"\">4 ON</a>"));
          client.println(F("<a href=\"/?r4off\"\">4 OFF</a><br><br><br>"));


          if (readString.indexOf("?r1on") > 0) {
            SetRelay(RELAY_1, RELAY_ON);
          }
          if (readString.indexOf("?r1off") > 0) {
            SetRelay(RELAY_1, RELAY_OFF);
          }
          if (readString.indexOf("?r2on") > 0) {
            SetRelay(RELAY_2, RELAY_ON);
          }
          if (readString.indexOf("?r2off") > 0) {
            SetRelay(RELAY_2, RELAY_OFF);
          }
          if (readString.indexOf("?r3on") > 0) {
            SetRelay(RELAY_3, RELAY_ON);
          }
          if (readString.indexOf("?r3off") > 0) {
            SetRelay(RELAY_3, RELAY_OFF);
          }
          if (readString.indexOf("?r4on") > 0) {
            SetRelay(RELAY_4, RELAY_ON);
          }
          if (readString.indexOf("?r4off") > 0) {
            SetRelay(RELAY_4, RELAY_OFF);
          }
          if (readString.indexOf("?r0on") > 0) {
            SetRelay(RELAY_1, RELAY_ON);
            SetRelay(RELAY_2, RELAY_ON);
            SetRelay(RELAY_3, RELAY_ON);
            SetRelay(RELAY_4, RELAY_ON);
          }
          if (readString.indexOf("?r0off") > 0) {
            SetRelay(RELAY_1, RELAY_OFF);
            SetRelay(RELAY_2, RELAY_OFF);
            SetRelay(RELAY_3, RELAY_OFF);
            SetRelay(RELAY_4, RELAY_OFF);
          }

          String s = "";
          for (int n = RELAY_MIN; n <= RELAY_MAX; ++n)
            s = s + relaystate[n] + " ";
            
          client.println("STATUS: " + s);

          client.println(F("</BODY>"));
          client.println(F("</HTML>"));

          //stopping client
          client.stop();
          
          //clearing string for next read
          readString = "";             
        }
      }
    }
  }
}
