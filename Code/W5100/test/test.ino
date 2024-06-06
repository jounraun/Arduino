#include <SPI.h>
#include <Ethernet.h>
#include <utility/w5100.h>

// change ip, gateway, and subnet to your network settings
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x59, 0x66 };  
byte ip[] = { 192, 168, 0, 123 };    
byte gateway[] = { 192, 168, 0, 254 };
byte subnet[] = { 255, 255, 255, 0 };

byte ipBuf[4];
char outBuf[18];

void setup()
{
 // new code
// pinMode(10,OUTPUT);
// digitalWrite(10,HIGH);

 Serial.begin(9600);
 Ethernet.begin(mac, ip, gateway, subnet);
 delay(1000);
 W5100.getIPAddress(ipBuf);
 sprintf(outBuf,"%u.%u.%u.%u\r\n",ipBuf[0],ipBuf[1],ipBuf[2],ipBuf[3]);
 Serial.write(outBuf);
}

void loop()
{
 Serial.println("Tick");
 delay(1000);
}
