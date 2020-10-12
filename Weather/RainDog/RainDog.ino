/*
 * RainDog
 * (c) 2016 jtr
 * UNO R3
 */

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

#define RESET_COUNTER 10000
#define DELAY_SECOND  1000

void(* resetFunc) (void) = 0;

int counter = 0;

void setup() {
  Serial.begin(9600);  
  Serial.println("*** Arduino");
  Serial.println("*** Rain Dog");
  Serial.println("*** (c) 2015 jtr");
  Serial.println("");
}

void loop() {
  // read the sensor on analog A0:
  int sensorReading = 1023 - analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  Serial.print(String(++counter) + ". " + sensorReading + " : ");

  // range value:
  switch (range) {
  case 2:    // Sensor getting wet
    Serial.println("Flood");
    break;
  case 1:    // Sensor getting wet
    Serial.println("Rain Warning");
    break;
  case 0:    // Sensor dry - To shut this up delete the " Serial.println("Not Raining"); " below.
    Serial.println("Not Raining");
    break;
  }

  for (int n = 1; n <= 1; ++n)
    delay(10 * DELAY_SECOND);

  if (counter == RESET_COUNTER) {
    Serial.println("Rebooting....");
    delay(DELAY_SECOND);
    resetFunc();
  }
}
