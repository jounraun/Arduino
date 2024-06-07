/* UDP4xRelay 
 *  
 */

// ENC28J60 Module
#include <UIPEthernet.h>
#include <UIPUdp.h>
#define UDP_TX_PACKET_MAX_SIZE 24

byte mac[] = {
  0xD1, 0xA2, 0xB3, 0xE4, 0xF5, 0xE6
};
IPAddress ip(192, 168, 0, 156);
unsigned int localPort = 4567;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

EthernetUDP Udp;

#define RELAY_ENQ 0
#define RELAY_MIN 1
#define RELAY_MAX 4
#define RELAY_OFF 0
#define RELAY_ON  1

int relaystate[RELAY_MAX + 1];
int relaypin[RELAY_MAX + 1] = {0, 4, 5, 6, 7};

void SetRelay(int n, int state)
{
  relaystate[n] = state;
  digitalWrite(relaypin[n], state);
}

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(9600);

  for (int n = RELAY_MIN; n <= RELAY_MAX; ++n) {
    pinMode(relaypin[n], OUTPUT);
    SetRelay(n, RELAY_OFF);
  }
}

void DoPacket(char *packet)
{
  char reply[10];
  char buf[10];
  int relay;
  int state;
  int i = 0;
  
  buf[0] = packet[0];
  buf[1] = '\0';
  relay = atoi(buf);

  buf[0] = packet[2];
  buf[1] = '\0';
  state = atoi(buf);

  if (((relay >= RELAY_MIN && relay <= RELAY_MAX) ||
       (relay == RELAY_ENQ)) &&
      (state == RELAY_OFF || state == RELAY_ON)) {

    if (relay != RELAY_ENQ) {
      SetRelay(relay, state);
    }

    for (int n = RELAY_MIN; n <= RELAY_MAX; ++n) {
      reply[i++] = '0' + relaystate[n];
      reply[i++] = ' ';
    }
    reply[i] = '\0';

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(reply);
    Udp.endPacket();
  }
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    DoPacket(packetBuffer);
  }
  delay(10);
}

