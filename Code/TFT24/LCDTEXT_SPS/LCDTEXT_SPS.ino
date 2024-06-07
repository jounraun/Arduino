#define LCD_CS A3    
#define LCD_CD A2    
#define LCD_WR A1   
#define LCD_RD A0    
#define LCD_RESET A4

#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF

#include "TFTLCD7781.h"

TFTLCD7781 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
//  Serial.begin(9600);
//  Serial.println("8 Bit LCD test!");
  
  
  tft.reset();
  tft.initDisplay();
  tft.fillScreen(BLACK);
  tft.setRotation(1); 
  testtext(10);

//  for (uint8_t rotation=0; rotation<4; rotation++) {


}

void loop(void) {

  delay(2000);

}

void testtext(uint16_t hum) {

  tft.fillScreen(BLACK);
  tft.setTextColor(BLUE);
  tft.setTextSize(5);
  tft.println("TaUrsa");
  tft.println("");

  tft.setTextSize(3);
  tft.setTextColor(GREEN);
  tft.println("Weather Station");
  tft.println("");

  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.println("Humidity    : 18%");
  tft.println("Temperature : +23C");
 
}

