
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 12
//      CS   - Pin 11
//

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

//Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK, DIN, DC, CS, RST);

Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);


void setup()   
{
  Serial.begin(9600);

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  
  for (int i = 0; i < 100; ++i) {  
  display.println(i);
  display.display();
  delay(1000);
  }
  

//  display.setTextColor(WHITE, BLACK); // 'inverted' text
//  display.println(3.141592);
//  display.setTextSize(2);
//  display.setTextColor(BLACK);
//  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();

}


void loop() 
{
  
}
