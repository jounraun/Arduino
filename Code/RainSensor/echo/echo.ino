#include <VirtualWire.h>

#define LEDPIN 13
#define MYBUFSIZE 80
char msg[MYBUFSIZE];

const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

void setup()
{
  Serial.begin(9600);	  // Debugging only
   
  vw_set_tx_pin(12);          // Sets pin D12 as the TX pin8
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(4000);	        // Bits per sec
}

void loop()
{
  rain_sensor();
  delay(1000);
}

void rain_sensor()
{
  String str;
  int sensorReading = analogRead(A0);
//  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  str = "RAIN " + String(sensorReading);

/*
  str = "N/A";
  switch (range) {
  case 0:
    str = "Heavy Rain (RS) : " + String(sensorReading);
    break;
  case 1:
    str = "Light Rain (RS) : " + String(sensorReading);
    break;
  case 2:
    str = "Not Raining (RS) : " + String(sensorReading);
    break;
  }
*/
  sendmsg(str);
  delay(5000);
}
  
void sendmsg(String str)
{
  digitalWrite(LEDPIN, true);  // Flash a light to show sending message
  Serial.println(str);
  str.toCharArray(msg, MYBUFSIZE);
  vw_send((uint8_t *)msg, strlen(msg));  //Sending the message
  vw_wait_tx();                         // Wait until the whole message is gone
  digitalWrite(LEDPIN, false);
  delay(1000);
}

