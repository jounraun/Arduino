/*
 * UIPEthernet EchoServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.1.6 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a TCP connection to
 * the Arduino.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 * Adaption to Enc28J60 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#define MACADDRESS 0x00,0x01,0x02,0x03,0x04,0x05
#define MYIPADDR 192,168,0,123
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,0,254
#define MYGW 192,168,0,254
#define LISTENPORT 1000

#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPClient.h>

uint8_t mac[6] = {MACADDRESS};
uint8_t myIP[4] = {MYIPADDR};
uint8_t myMASK[4] = {MYIPMASK};
uint8_t myDNS[4] = {MYDNS};
uint8_t myGW[4] = {MYGW};

EthernetServer server = EthernetServer(LISTENPORT);

void setup()
{
  Ethernet.begin(mac, myIP, myDNS, myGW, myMASK);
  server.begin();
}

void loop()
{
  size_t size;

  if (EthernetClient client = server.available()) {
    if (client) {
      while ((size = client.available()) > 0) {
        uint8_t* msg = (uint8_t*) malloc(size);
        size = client.read(msg, size);
        client.write(msg, size);
        free(msg);
      }
    }
  }
}
