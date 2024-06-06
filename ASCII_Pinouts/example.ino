/*
 
                                      +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |           GND/RST2  [ ] [ ]         |
         |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   C5
         |            5V/MISO2 [ ] [ ]  SDA[ ] |   C4
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                    SCK/13[A] |   B5
         | [ ]v.ref                 MISO/12[A] |   .
         | [ ]RST                   MOSI/11[A]~|   .
         | [ ]3V3    +---+               10[ ]~|   .
         | [ ]5v     | A |                9[ ]~|   .
         | [ ]GND   -| R |-               8[B] |   B0
         | [ ]GND   -| D |-                    |
         | [ ]Vin   -| U |-               7[A] |   D7
         |          -| I |-               6[A]~|   .
         | [ ]A0    -| N |-               5[C]~|   .
         | [ ]A1    -| O |-               4[A] |   .
         | [ ]A2     +---+           INT1/3[A]~|   .
         | [ ]A3                     INT0/2[ ] |   .
         | [ ]A4      RST SCK MISO     TX>1[ ] |   .
         | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
         |            [ ] [ ] [ ]              |
         |  UNO_R3    GND MOSI 5V  ____________/
          \_______________________/
		  
		  http://busyducks.com/ascii-art-arduinos
*/
 
//------------------------------------------------------------------ 
// [A]		Adafruit music shield
//------------------------------------------------------------------ 
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// SPI Clock, shared with SD card
#define CLK_PIN (13)       
// Input data, from VS1053/SD card
#define MISO_PIN (12)      
// Output data, to VS1053/SD card
#define MOSI_PIN (11)      
 
// VS1053 reset pin (unused!)
#define SHIELD_RESET_PIN  (-1)   
// VS1053 chip select pin (output)
#define SHIELD_CS_PIN     (7)      
// VS1053 Data/command select pin (output)
#define SHIELD_DCS_PIN    (6)      
// Card chip select pin
#define CARDCS_PIN (4)     
// VS1053 Data request, ideally an Interrupt pin
#define DREQ_PIN (3)       
 
//------------------------------------------------------------------  
// [B]		WS2811 LED stip
//------------------------------------------------------------------ 
#define LED_STIP_PIN  (8)
 
//------------------------------------------------------------------  
// [C]		Servo Motor
//------------------------------------------------------------------ 
#define SERVO_PIN (5)
