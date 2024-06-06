/*************************

MS1  MS2  Resolution
--------------------
L    L    Full Step
H    L    Half Step
L    H    Quarter Step
H    H    Eigth Step

***************************/

#define SERIALNO "01"

#define DELAY_SECOND 1000

#define FORWARD 1
#define REVERSE 0

#define PIN_STEP 2
#define PIN_DIR  3
#define PIN_EN   4

int direction = FORWARD;

#define MAXSTEPS   1000
#define MINSTEPS   1
#define BIGSTEP    10
#define SMALLSTEP  1

int steps = MAXSTEPS / 2;

void resetEDPins()
{
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_DIR, LOW);
  digitalWrite(PIN_EN, HIGH);
}

void move()
{
  Serial.println("MOVE");

  digitalWrite(PIN_EN, LOW);

  for (int i = 0; i < steps; i++) {
    digitalWrite(PIN_STEP, LOW);
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(500);
  }

  digitalWrite(PIN_EN, HIGH);
}

void setup()
{

  Serial.begin(9600);
  Serial.println("*** Arduino");
  Serial.println("*** EasyStepper");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_EN, OUTPUT);

  resetEDPins();

  digitalWrite(PIN_DIR, HIGH);
}

void showhelp()
{
  Serial.println("u/d/U/D/m/+/-/h");
}

void incsteps(int val)
{
  steps += val;
  if (steps > MAXSTEPS) {
    steps = MAXSTEPS;
    Serial.println("Reached maximum value!");
  } else {
    Serial.println("INC by " + String(val));
  }  
}

void decsteps(int val)
{
  steps -= val;
  if (steps < MINSTEPS) {
    steps = MINSTEPS;
    Serial.println("Reached minimum value!");
  } else {
    Serial.println("DEC by " + String(val));
  }  
}

void toggle()
{
  if (direction == FORWARD) {
    direction = REVERSE;
    digitalWrite(PIN_DIR, LOW);
    Serial.println("Direction set to REVERSE.");
  } else {
    direction = FORWARD;
    digitalWrite(PIN_DIR, HIGH);
    Serial.println("Direction set to FORWARD.");
  }
}

void loop()
{
  int i;

  if (Serial.available()) {
    
    char input = Serial.read();  
    if (input != -1) {

      switch (input) {

      case 'h':
        showhelp();
        break;        

      case 'm':
         move();
         break;

      case 't':
         toggle();
         break;
         
      case '+':
        if (direction == FORWARD)
           toggle();
        move();
        break;        

      case '-':
        if (direction == REVERSE)
           toggle();
        move();
        break;

      case 'U':
        incsteps(BIGSTEP);
        break;

      case 'D':
        decsteps(BIGSTEP);
        break;

      case 'u':
        incsteps(SMALLSTEP);
        break;

      case 'd':
        decsteps(SMALLSTEP);
        break;
        
      default:
        if (input != '\r' && input != '\n')
          Serial.println("Invalid entry!");
        break;
      }
    }
  }

  Serial.println("Speed = " + String(steps));
  
  for (int n = 0; n < 1000; ++n) {
    if (Serial.available())
      break;
    delay(DELAY_SECOND / 100);
  }

}


