/*
 * AstroRelay
 * (c) 2016 jtr
 * UNO R3
 */

byte relayPin[4] = { 7, 6, 5, 4 };
char input=0;
int val;

//#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

void setup() {
  
  for (int i = 0; i < 4; i++) pinMode(relayPin[i], OUTPUT);
  Serial.begin(9600); 

  Serial.println("*** Arduino");
  Serial.println("*** Astro Relay");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");
  
//  for (int j = 0; j < 4; j++) digitalWrite(relayPin[j], LOW);

  showhelp();
  showrelay(); 
}

void loop() {

  if (Serial.available()) {
    
    char input= Serial.read();  
    if (input != -1) {
      switch (input) {

      case 'h':                         
        showhelp();
        break;

      case 'r':                         
        reboot();
        break;

      case '1':                         
        Serial.print("Relay #1: ");
        val = digitalRead(relayPin[0]);
        val = !val;
        Serial.println(val ? "ON" : "OFF");
        digitalWrite(relayPin[0], val);
        break;
        
      case '2':
        Serial.print("Relay #2: ");
        val = digitalRead(relayPin[1]);
        val = !val;
        Serial.println(val ? "ON" : "OFF");
        digitalWrite(relayPin[1], val);
        break;
        
      case '3':
        Serial.print("Relay #3: ");
        val = digitalRead(relayPin[2]);
        val = !val;
        Serial.println(val ? "ON" : "OFF");
        digitalWrite(relayPin[2], val);
        break;
        
      case '4':
        Serial.print("Relay #4: ");
        val = digitalRead(relayPin[3]);
        val = !val;
        Serial.println(val ? "ON" : "OFF");
        digitalWrite(relayPin[3], val);
        break;    
        
      default:
        if (input != '\r' && input != '\n')
          Serial.println("Invalid entry!");
        break;
      }
      showrelay();
    }
  }

  delay (DELAY_SECOND);

//  if (++counter == RESET_COUNTER) {
//    reboot();
//  }
}

void showrelay()
{ 
  Serial.println();
  Serial.println(String("1 = ") + (digitalRead(relayPin[0]) ? "ON" : "OFF"));
  Serial.println(String("2 = ") + (digitalRead(relayPin[1]) ? "ON" : "OFF"));
  Serial.println(String("3 = ") + (digitalRead(relayPin[2]) ? "ON" : "OFF"));
  Serial.println(String("4 = ") + (digitalRead(relayPin[3]) ? "ON" : "OFF"));
  Serial.println();
}

void showhelp()
{
  Serial.println("Press 1/2/3/4/h/r to control");
  Serial.println("the state of the relay.");
}

void reboot()
{
  Serial.println("Rebooting....");
  delay (DELAY_SECOND);
  resetFunc();
}
