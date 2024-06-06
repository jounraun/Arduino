/*
 Created by Rui Santos
 Visit: http://randomnerdtutorials.com for more arduino projects

 Arduino with Ethernet Shield
 */

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 0, 123 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 254 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask

EthernetServer server(80);                             //server port     

#define RELAY_ON 1
#define RELAY_OFF 0
#define Relay_1  5
#define Relay_2  6
#define Relay_3  7
#define Relay_4  8


String readString;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
    
  pinMode(Relay_1, OUTPUT);   
  pinMode(Relay_2, OUTPUT);  
  pinMode(Relay_3, OUTPUT);  
  pinMode(Relay_4, OUTPUT);    
  
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
     
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();     
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
          client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
          client.println("<link rel='stylesheet' type='text/css' href='http://acerlink.dyndns.info:2344/Web_Relay.css' />");
          client.println("<TITLE>Web Relay</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<H1>Web Relay</H1>");
          client.println("<hr />");
          client.println("<br />");  
          client.println("<H2>4-channel Arduino Relay</H2>");
          client.println("<br />");  
          client.println("<br />");  
          client.println("<a href=\"/?button1on\"\">1 ON</a>");
          client.println("<a href=\"/?button1off\"\">1 OFF</a><br />");   
          client.println("<br />");  
          client.println("<br />");     
          client.println("<a href=\"/?button2on\"\">2 ON</a>");
          client.println("<a href=\"/?button2off\"\">2 OFF</a><br />"); 
          client.println("<br />");  
          client.println("<br />");     
          client.println("<a href=\"/?button3on\"\">3 ON</a>");
          client.println("<a href=\"/?button3off\"\">3 OFF</a><br />"); 
          client.println("<br />");  
          client.println("<br />");     
          client.println("<a href=\"/?button4on\"\">4 ON</a>");
          client.println("<a href=\"/?button4off\"\">4 OFF</a><br />"); 
          client.println("<br />");  
          client.println("<br />");     
          client.println("<a href=\"/?buttonALLon\"\">All ON</a>");
          client.println("<a href=\"/?buttonALLoff\"\">All OFF</a><br />"); 

          client.println("<br />"); 
          client.println("</BODY>");
          client.println("</HTML>");
     
          delay(1);
          //stopping client
          client.stop();
          //controls the Arduino if you press the buttons

          if (readString.indexOf("?button1on") > 0) {
            Serial.println("1 ON");
            digitalWrite(Relay_1, RELAY_ON);
          }
          if (readString.indexOf("?button1off") > 0) {
            Serial.println("1 OFF");
            digitalWrite(Relay_1, RELAY_OFF);
          }
          if (readString.indexOf("?button2on") > 0) {
            Serial.println("2 ON");
            digitalWrite(Relay_2, RELAY_ON);
          }
          if (readString.indexOf("?button2off") > 0) {
            Serial.println("2 OFF");
            digitalWrite(Relay_2, RELAY_OFF);
          }
          if (readString.indexOf("?button3on") > 0) {
            Serial.println("3 ON");
            digitalWrite(Relay_3, RELAY_ON);
          }
          if (readString.indexOf("?button3off") > 0) {
            Serial.println("3 OFF");
            digitalWrite(Relay_3, RELAY_OFF);
          }
          if (readString.indexOf("?button4on") > 0) {
            Serial.println("4 ON");
            digitalWrite(Relay_4, RELAY_ON);
          }
          if (readString.indexOf("?button4off") > 0) {
            Serial.println("4 OFF");
            digitalWrite(Relay_4, RELAY_OFF);
          }
          if (readString.indexOf("?buttonALLon") > 0) {
            Serial.println("All ON");
            digitalWrite(Relay_1, RELAY_ON);
            digitalWrite(Relay_2, RELAY_ON);
            digitalWrite(Relay_3, RELAY_ON);
            digitalWrite(Relay_4, RELAY_ON);
          }
          if (readString.indexOf("?buttonALLoff") > 0) {
            Serial.println("All OFF");
            digitalWrite(Relay_1, RELAY_OFF);
            digitalWrite(Relay_2, RELAY_OFF);
            digitalWrite(Relay_3, RELAY_OFF);
            digitalWrite(Relay_4, RELAY_OFF);
          }

          //clearing string for next read
          readString="";             
        }
      }
    }
  }
}

