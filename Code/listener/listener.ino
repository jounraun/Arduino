#include <VirtualWire.h>

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


int count;

String str1 = "";
String str2 = "";
String str3 = "";
String str4 = "";
String str5 = "";
String str6 = "";

void setup()
{
    Serial.begin(9600);	// Debugging only

    vw_set_rx_pin(7);       //Sets pin D12 as the RX Pin
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(4000);	     // Bits per sec
    vw_rx_start();       // Start the receiver PLL running

  display.begin();
  display.clearDisplay();
  display.setContrast(50);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.display();

/*  
  display.setCursor(0,0);
  display.println("eka");
  display.display();
  display.setCursor(40,10);
  display.println("toka");
  display.display();
  delay(1000);

  display.setCursor(40,10);
  display.println("    ");
  display.display();
  delay(1000);

  display.setCursor(40,10);
  display.println("TOKA");
  display.display();

  delay(1000);
*/
}

void loop()
{
  String str;
  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) {
    digitalWrite(13, true);
    str = "";
    for (int i = 0; i < buflen; i++) {
      char c = (buf[i]);
      str = str + String(c);
    }
    digitalWrite(13, false);
    Serial.println(str);
  }
  str1 = str2;
  str2 = str3;
  str3 = str4;
  str4 = str5;
  str5 = str6;
  str6 = str;
 
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(str1);
  display.println(str2);
  display.println(str3);
  display.println(str4);
  display.println(str5);
  display.println(str6);
  display.display();
  delay(1000);
}
