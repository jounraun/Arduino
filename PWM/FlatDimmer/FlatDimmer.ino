/*
 * FlatDimmer
 * (c) 2017 jtr
 * Nano
 */

#define PWM_MIN   0
#define PWM_MAX   255
#define PWM_STEP  10

/* Alnilam */
//#define SERIALNO "04"

/* Auriga */
#define SERIALNO "03"

const int PWM_Pin = 9;
char input = 0;
int pwmval = PWM_MIN;

#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

void setup()
{
  Serial.begin(9600); 

  Serial.println("*** Arduino");
  Serial.println("*** Flat Dimmer");
  Serial.print("*** SN #");
  Serial.println(SERIALNO);
  Serial.println("*** (c) 2017 jtr");
  Serial.println("");

  pinMode(PWM_Pin, OUTPUT);

  showhelp();  
  showpwm();
  
}

void loop()
{
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
   
      case 'u':
        pwmval += PWM_STEP;
        if (pwmval > PWM_MAX) {
          pwmval = PWM_MAX;
          Serial.println("Maximum value reached!");
        }
        break;        

      case 'd':
        pwmval -= PWM_STEP;
        if (pwmval < PWM_MIN) {
          pwmval = PWM_MIN;
          Serial.println("Minimum value reached!");
        }
        break;        

      case '+':
        pwmval += 1;
        if (pwmval > PWM_MAX) {
          pwmval = PWM_MAX;
          Serial.println("Maximum value reached!");
        }
        break;        

      case '-':
        pwmval -= 1;
        if (pwmval < PWM_MIN) {
          pwmval = PWM_MIN;
          Serial.println("Minimum value reached!");
        }
        break;        

      case '0':
        pwmval = PWM_MIN;
        break;        

      case '1':
        pwmval = PWM_MAX;
        break;        

      default:
        if (input != '\r' && input != '\n')
          Serial.println("Invalid entry!");
        break;
      }

      showpwm();
    }
  }

  delay (DELAY_SECOND / 10);
}

void showpwm()
{ 
  Serial.println("PWM = " + String(pwmval));
  analogWrite(PWM_Pin, pwmval);  
}

void showhelp()
{
  Serial.println("Press +/-/u/d/1/0/h/r to control");
  Serial.println("the state of the panel.");
  Serial.println("");
}

void reboot()
{
  Serial.println("Rebooting....");
  delay (DELAY_SECOND);
  resetFunc();
}

