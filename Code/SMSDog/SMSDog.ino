bool SMS_SIMULATE = false;

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

#include <DHT.h>

#define DHTPIN 10

#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

#define BAUDRATE 9600

#define SMSBUFSIZE 160
#define MAXSMS 20
int numdata;
boolean started = false;
char n[MAXSMS];

#define PHONENUMBERLEN 20
char phone_number[PHONENUMBERLEN];
int sms_position;
char sms_text[SMSBUFSIZE + 1];
int i;

#define GSM_ME    "0458030525"
#define GSM_SALDO "14000"

#define MIN_TEMP 5
#define MAX_TEMP 35

int ack = false;
int sent = false;
float t = 0;
float t1 = MIN_TEMP + 10;
float t2 = MIN_TEMP + 10;
float t3 = MIN_TEMP + 10;
float t4 = MIN_TEMP + 10;
float t5 = MIN_TEMP + 10;

#define RESET_COUNTER 10000

#define DELAY_SECOND 1000
#define DELAY_MINUTE 60 * DELAY_SECOND

int rstcnt = 0;
void(* resetFunc) (void) = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("SMS Dog starting...");

  dht.begin();

  if (gsm.begin(9600)) {
    Serial.println("GSM started.");
    started = true;

    Serial.println("Deleting all SMS messages...");
    for (i = 1; i <= 20; i++) {
      Serial.print(i);
      Serial.print(" ");
      sms.DeleteSMS(i);
    }
    Serial.println("Done.");
    Serial.println("Sending start message.");
    SMS_Send(GSM_ME, "SMS Dog Started.");
  } else {
    Serial.println("GSM idle.");
  }
}

void loop()
{
  char msg[SMSBUFSIZE + 1];

  t = dht.readTemperature();

  Serial.print(++rstcnt);
  Serial.print("\t");
  Serial.println(t);

  t5 = t4;
  t4 = t3;
  t3 = t2;
  t2 = t1;
  t1 = t;

  t = (t1 + t2 + t3 + t4 + t5) / 5;

  if (started) {

    if ((t > MAX_TEMP || t < MIN_TEMP) && !sent) {
      if (t > MAX_TEMP)
        sprintf(msg, "Temperature too high : %f C", t);
      if (t < MIN_TEMP)
        sprintf(msg, "Temperature too low : %f C", t);
      SMS_Send(GSM_ME, msg);
      sent = true;
    }

    sms_position = sms.IsSMSPresent(SMS_UNREAD);
    if (sms_position >= 1) {

      SMS_Receive();

      if (strcasecmp(sms_text, "OK") == 0) {
        Serial.println("Acknowledge received.");
        ack = true;
        sent = false;
      }

      if (strcasecmp(sms_text, "REBOOT") == 0) {
        Serial.println("Rebooting...");
        delay(DELAY_SECOND);
        resetFunc();
      }

      if (strcasecmp(sms_text, "STATUS") == 0) {
        Serial.println("Sending status message.");
        GetSaldo();
        int ti = ((t + 0.5) * 100) / 100;
        sprintf(msg, "Temperature %d C, Uptime %d min, %s", ti, rstcnt, sms_text);
        SMS_Send(GSM_ME, msg);
      }

    }
  }

  if (rstcnt == RESET_COUNTER) {
    Serial.println("Rebooting....");
    delay(DELAY_SECOND);
    resetFunc();
  }

  for (i = 1; i <= 6; ++i)
    delay(10 * DELAY_SECOND);
}

void SMS_Send(char *number, char *msg)
{
  Serial.println("Sending message...");
  Serial.println("Number  : " + String(number));
  if (!SMS_SIMULATE) {
    sms.SendSMS(number, msg);
  }
  Serial.println("Message : " + String(msg));
  Serial.println("Done.")  ;
}

void SMS_Receive()
{
  Serial.println("Receiving message...");
  Serial.println("Position : " + String(sms_position));
  sms.GetSMS(sms_position, phone_number, sms_text, SMSBUFSIZE);
  Serial.println("Number   : " + String(phone_number));
  Serial.println("Message  : " + String(sms_text));
  Serial.println("Done.");
}

void GetSaldo()
{
  bool received = false;

  if (SMS_SIMULATE) {
    strcpy(sms_text, "Saldosi on 12.34 EUR.");
  } else {
    sms.SendSMS(GSM_SALDO, "SALDO");
    while (!received) {
      sms_position = sms.IsSMSPresent(SMS_UNREAD);
      if (sms_position >= 1) {
        sms.GetSMS(sms_position, phone_number, sms_text, SMSBUFSIZE);
        received = true;
      } else {
        delay(DELAY_SECOND);
      }
    }
  }
}

