
/*************************
Joel Bartlett
SparkFun Electronics
December 27, 2012

This code controls a stepper motor with the 
EasyDriver board. It spins forwards and backwards

https://www.sparkfun.com/tutorials/400

http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html

***************************/
int dirpin = 8;
int steppin = 9;

void setup() 
{
pinMode(dirpin, OUTPUT);
pinMode(steppin, OUTPUT);
}
void loop()
{

  int i;

  digitalWrite(dirpin, LOW);     // Set the direction.
  delay(100);


  for (i = 0; i<4000; i++)       // Iterate for 4000 microsteps.
  {
    digitalWrite(steppin, LOW);  // This LOW to HIGH change is what creates the
    digitalWrite(steppin, HIGH); // "Rising Edge" so the easydriver knows to when to step.
    delayMicroseconds(500);      // This delay time is close to top speed for this
  }                              // particular motor. Any faster the motor stalls.

  digitalWrite(dirpin, HIGH);    // Change direction.
  delay(100);


  for (i = 0; i<4000; i++)       // Iterate for 4000 microsteps
  {
    digitalWrite(steppin, LOW);  // This LOW to HIGH change is what creates the
    digitalWrite(steppin, HIGH); // "Rising Edge" so the easydriver knows to when to step.
    delayMicroseconds(500);      // This delay time is close to top speed for this
  }                              // particular motor. Any faster the motor stalls.

}
