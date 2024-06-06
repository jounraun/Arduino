// Define which pin to be used to communicate with Base pin of TIP120 transistor

int TIP120pin = 11; //for this project, I pick Arduino's PMW pin 11

void setup()
{
  pinMode(TIP120pin, OUTPUT); // Set pin for output to control TIP120 Base pin
  analogWrite(TIP120pin, 255); // By changing values from 0 to 255 you can control motor speed
}

void loop()
{
}

