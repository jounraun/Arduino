#include <VirtualWire.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 222);
IPAddress gateway(192, 168, 0, 254);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server(80);

#define STRDATE "DATE"
#define STRTIME "TIME"
#define STRTMP1 "TMP1"
#define STRTMP2 "TMP2"
#define STRTMP3 "TMP3"
#define STRPRES "PRES"
#define STRHUMD "HUMD"
#define STRRAIN "RAIN"
#define STRLGHT "LGHT"
#define STRIR   "IR  "

#define MYBUFSIZE 80
char mybuf[MYBUFSIZE];

String strdate = "";
String strtime = "";
String strtmp1 = "";
String strtmp2 = "";
String strtmp3 = "";
String strpres = "";
String strhumd = "";
String strrain = "";
String strlght = "";
String strir   = "";

void setup() {

  SPI.begin();
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);

  vw_set_rx_pin(8);       //Sets pin D12 as the RX Pin
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(4000);	     // Bits per sec
  vw_rx_start();       
    
  delay(1000);
}

void loop() {  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) {
    String str;
    char c;
    for (int i = 0; i < buflen; i++) {
      c = (buf[i]);
      str = str + c;
    }
    Serial.println(str);
    str.toCharArray(mybuf, MYBUFSIZE);

    if (!strncmp(mybuf, STRDATE, strlen(STRDATE))) {
       strdate = str;
    }
    if (!strncmp(mybuf, STRTIME, strlen(STRTIME))) {
       strtime = str;
    }
    if (!strncmp(mybuf, STRTMP1, strlen(STRTMP1))) {
       strtmp1 = str;
    }
    if (!strncmp(mybuf, STRTMP2, strlen(STRTMP2))) {
       strtmp2 = str;
    }
    if (!strncmp(mybuf, STRTMP3, strlen(STRTMP3))) {
       strtmp3 = str;
    }
    if (!strncmp(mybuf, STRPRES, strlen(STRPRES))) {
       strpres = str;
    }
    if (!strncmp(mybuf, STRHUMD, strlen(STRHUMD))) {
       strhumd = str;
    }
    if (!strncmp(mybuf, STRRAIN, strlen(STRRAIN))) {
       strrain = str;
    }
    if (!strncmp(mybuf, STRLGHT, strlen(STRLGHT))) {
       strlght = str;
    }
    if (!strncmp(mybuf, STRIR, strlen(STRIR))) {
       strir = str;
    }

  }
  listenForEthernetClients();
}

void listenForEthernetClients() {
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
          client.print(strdate);
          client.println("<br />");
          client.print(strtime);
          client.println("<br />");
          client.print(strtmp1);
          client.println("<br />");
          client.print(strtmp2);
          client.println("<br />");
          client.print(strtmp3);
          client.println("<br />");
          client.print(strpres);
          client.println("<br />");
          client.print(strhumd);
          client.println("<br />");
          client.print(strrain);
          client.println("<br />");
          client.print(strlght);
          client.println("<br />");
          client.print(strir);
          client.println("<br />");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
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
