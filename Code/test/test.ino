#define RESET_COUNTER 10
#define DELAY_SECOND  1000

int rstcnt = 0;

void(* resetFunc) (void) = 0;

void setup(void)
{
  Serial.begin(9600);
}

void loop(void)
{ 
  Serial.println(String(++rstcnt) + ". " + "123 " + "C");  

  if (rstcnt == RESET_COUNTER) {
    Serial.println("Rebooting....");
    delay(DELAY_SECOND);
    resetFunc();
  }

  for (int n = 1; n <= 10; ++n)
    delay(DELAY_SECOND);
}
