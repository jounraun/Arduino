/* YourDuino.com Example: Multiple DS18B20 Temperature Sensors
   Displayed on 4x20 character LCD display
   
   DS18B20 Pinout (Left to Right, pins down, flat side toward you)
  - Left   = Ground
  - Center = Signal (Pin 10):  (with 3.3K to 4.7K resistor to +5 or 3.3 )
  - Right  = +5 or +3.3 V
   
   terry@yourduino.com */

/*-----( Import needed libraries )-----*/
// Get 1-wire Library here: http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

//Get DallasTemperature Library here:  http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>
// Wire (I2C) Library
#include <Wire.h>
// LCD Library
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library
//Download: https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move original LiquidCrystal library elsewhere, copy this in it's place


/*-----( Declare Constants and Pin Numbers )-----*/
// Data wire is plugged into port 10 on the Arduino (can be changed)
#define ONE_WIRE_BUS 10    // NOTE: No ";" on #define  

#define I2C_ADDR    0x20  // Define I2C Address for the PCF8574A on the LCD Backpack board
//---(Following are the PCF8574 pin assignments to LCD connections )----
// This are different than earlier/different I2C LCD displays
#define BACKLIGHT_PIN  7
#define En_pin  4
#define Rw_pin  5
#define Rs_pin  6
#define D4_pin  0
#define D5_pin  1
#define D6_pin  2
#define D7_pin  3

#define  LED_OFF  0
#define  LED_ON  1

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass address of our oneWire instance to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Start the LCD display library
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

/*-----( Declare Variables )-----*/
// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20#Read%20individual


 // WP 1
DeviceAddress Probe01 = { 0x28, 0x9A, 0x80, 0x40, 0x04, 0x00, 0x00, 0xD5 }; // "4"
DeviceAddress Probe02 = { 0x28, 0xE1, 0xC7, 0x40, 0x04, 0x00, 0x00, 0x0D }; // "5"
DeviceAddress Probe03 = { 0x28, 0x9A, 0x80, 0x40, 0x04, 0x00, 0x00, 0xD5 }; // "4" Again for test
DeviceAddress Probe04 = { 0x28, 0x10, 0xA4, 0x57, 0x04, 0x00, 0x00, 0xA9 };


void setup()   /****** SETUP: RUNS ONCE ******/
{
//------- Initialize the Temperature measurement library--------------
  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 10);
  sensors.setResolution(Probe02, 10);
  sensors.setResolution(Probe03, 10);
  sensors.setResolution(Probe04, 10);

//---------------- Initialize the lcd ------------------  
  lcd.begin (20,4);  // 20 characters, 4 lines
// Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,NEGATIVE);
  lcd.setBacklight(LED_ON);  

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  sensors.requestTemperatures(); // Send the command to get temperatures

  lcd.clear();  // Reset the display  
  lcd.home();
  lcd.backlight();  //Backlight ON if under program control  
  
// Print our characters on the LCD
// NOTE: Line number and character number start at 0 not 1
  
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("1: ");
  displayTemperature(Probe01);  
  
  lcd.setCursor(0,1); //Start at character 0 on line 1
  lcd.print("2: ");
  displayTemperature(Probe02);  
  
  lcd.setCursor(0,2); //Start at character 0 on line 2
  lcd.print("3: ");
  displayTemperature(Probe03);  
  
  lcd.setCursor(0,3); //Start at character 0 on line 3
  lcd.print("4: ");
  displayTemperature(Probe04);  
  
  delay(2000);

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void displayTemperature(DeviceAddress deviceAddress)
{

float tempC = sensors.getTempC(deviceAddress);

   if (tempC == -127.00) // Measurement failed or no device found
   {
    lcd.print("Temperature Error");
   } 
   else
   {
   lcd.print("C=");
   lcd.print(tempC);
   lcd.print(" F=");
   lcd.print(DallasTemperature::toFahrenheit(tempC)); // Convert to F
   }
}// End printTemperature

//*********( THE END )***********