
//------------------------------------------------------------------
// Ascom-Arduino Focusser
// Dave Wells
// Thanks for code snippets & inspiration:
//  o  Gina (Stargazers Lounge) for the stepper control basics
//  o  russellhq  (Stargazers Lounge) for the 1-wire code and info.
//------------------------------------------------------------------

//------ Change Log ----------------------------------------------------------------------
//  Version    Date        Change
//  0.0.2      26/05/2014  Initial - copied from Windows version
//  2.0.0      22/06/2014  Renumbered to match Ascom Driver Numbering
//  2.0.1      09/08/2014  Initial position set to 1000
//  2.0.2      16/08/2014  Halt function implemented with H# command
//                         New I# command to set an initial position
//
//  version numbering brought into line with driver numbering
//  2.2.0      03/10/2014  Implemented Temperature Sensing C# command
//  2.3.0      07/10/2014  Now stores position in EEPROM
//                         Changed some int variables to unsigned int
//                         Fixed minor bug in motor hi/lo speed detection
//                         Temperature now in 100ths of degree for internationlisation
//                         Block temperature requests during focuser movement
// 2.4.0       18/04/2015  Amended to step in single half-steps not 8 per unit
//                         Moved user-configurable parameters to a single block at top of code
//                         Reduced number of read/write operations to/from EEPROM
//                         Reduced no of calls to clear the output pins in loop()
//                         Some general code and structure tidying
// 2.5.0       24/06/2015  Changes to integrate Bluetooth Comms using HC05.
//                         Some refactoring, renaming, rearranging of code
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Include necessary libraries
//----------------------------------------------------------------------------------------------------------------------
#include <OneWire.h>                                     // DS18B20 temp sensor
#include <DallasTemperature.h>                           // DS18B20 temp sensor  
#include <EEPROM.h>                                      // EEPROM Library
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// User-configurable values. Only change values if you know what you are doing.
//----------------------------------------------------------------------------------------------------------------------
//#define BLUETOOTH_COMMS                                   // Use Bluetooth Comms - if commented out defaults to USB Serial Port
#define MOTOR_PINS {7,8,9,10};                            // Declare pins to drive motor control board
#define ONE_WIRE_BUS                    6                 // DS18B20 DATA wire connected to Digital Pin 6
#define MOTOR_STEPS_PER_DRIVER_STEP     8                 // Motor steps per Ascom movement Unit (old method = 8)
#define MOTOR_SPEED_DEFAULT             16000             // Default motor step speed (uS)(failsafe operation)
#define MOTOR_SPEED_LO                  16000             // Motor step delay for Lo speed (uS)
#define MOTOR_SPEED_HI                  2000              // Motor step delay for Hi speed (uS)
#define MOTOR_SPEED_THRESHOLD           100               // motor speed Hi if steps to go is higher than this
#define DEFAULT_START_POSITION          5000              // Default Start Position if not set by Innnn# command
#define TEMP_RESOLUTION                 10                // 1-wire temperature sensor resolution 9=9bit(0.50C), 10=10bit(0.25C), 11=11bit(0.125C), 12=12bit(0.0625C)

// Additional Config if using Bluetooth Comms
#ifdef BLUETOOTH_COMMS
  #include <SoftwareSerial.h>                              // For Bluetooth Connection
  #define BTSERIAL_PWR_PIN                2                // Bluetooth serial Power Pin
  #define BTSERIAL_MODE_PIN               3                // Bluetooth serial Mode Pin
  #define BTSERIAL_RX_PIN                 4                // Bluetooth serial RX Pin
  #define BTSERIAL_TX_PIN                 5                // Bluetooth serial TX Pin
  #define SERIALPORT BTSerial
  const String BT_NAME = "Focuser";                        // Bluetooth Device Name
  const String BT_PWD  = "9999";                           // Bluetooth Device Password (Pairing Code)
  
  SoftwareSerial BTSerial(BTSERIAL_RX_PIN, BTSERIAL_TX_PIN); // SoftwareSerial Serial Port for Bluetooth module
#else
  #define SERIALPORT Serial
#endif
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// EEPROM storage parameters
//----------------------------------------------------------------------------------------------------------------------
#define EE_LOC_POS 0                                       // Location of position (2 bytes)
#define EE_LOC_PSTAT 2                                     // Location of Position Status (1 Byte)
#define POS_VALID 55                                       // Stored position valid if this value otherwise invalid
//----------------------------------------------------------------------------------------------------------------------

const String programName = "Arduino Focuser";
const String programVersion = "2.5.0";

const int motorPins[4] = MOTOR_PINS;                       // Declare pins to drive motor control board

int           motorSpeed = MOTOR_SPEED_DEFAULT;            // current delay for motor step speed (uS)
DeviceAddress tempSensor;                                  // Temperature sensor
double        currentTemperature;                          // current temperature
boolean       tempSensorPresent = false;                   // Is there a temperature sensor installed?
int           step = 0;                                    // current motor step position
boolean       outputActive = true;                         // Is motor energised? Initialised to true so it gets cleared on startup

// Default initial positions if not set using the Innnn# command by Ascom Driver
unsigned int           currentPosition = DEFAULT_START_POSITION;     // current position
unsigned int           targetPosition  = DEFAULT_START_POSITION;     // target position
unsigned int           lastSavedPosition;                            // last position saved to EEPROM

// Initialise the temp sensor
OneWire oneWire(ONE_WIRE_BUS);                             // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);                       // Pass our oneWire reference to Dallas Temperature.
  
// Lookup table to drive motor control board                                
const int stepPattern[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

// For ASCOM connection
String inputString = "";                                   // string to hold incoming data
boolean stringComplete = false;                            // whether the string is complete
boolean isMoving = false;                                  // is the motor currently moving


//=============================================================================================================
// Setup
//=============================================================================================================

void setup() 
{
  //-----------------------------------------------------------------------------------------------------------
  // Setup IO Pins
  //-----------------------------------------------------------------------------------------------------------

  // Motor Pins
  for (int i=0; i<4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  // Bluetooth (if defined)
  #ifdef BLUETOOTH_COMMS
    pinMode(BTSERIAL_MODE_PIN, OUTPUT); 
    digitalWrite(BTSERIAL_MODE_PIN, LOW); 

    pinMode(BTSERIAL_PWR_PIN, OUTPUT);
    digitalWrite(BTSERIAL_PWR_PIN, LOW); 
  #endif
  //-----------------------------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------------------------
  // Comms initialisation
  //-----------------------------------------------------------------------------------------------------------
  clearOutput();                                     // Ensure all motor coils de-energised
  inputString.reserve(200);                          // reserve 200 bytes for the ASCOM driver inputString
  SERIALPORT.begin(9600);                            // Initialize SERIALPORT for ASCOM interface
  //-----------------------------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------------------------
  // Position initialisation
  //-----------------------------------------------------------------------------------------------------------
  if (storedPositionValid()) {                       // Check if EEPROM position is valid
    currentPosition = restorePosition();             // Use position from EEPROM if it is valid
  } else {
    currentPosition = DEFAULT_START_POSITION;        // If invalid use the default position
  }
  lastSavedPosition = currentPosition;
  targetPosition = currentPosition;
  //-----------------------------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------------------------
  // OneWire Libary initialisation
  //-----------------------------------------------------------------------------------------------------------
  oneWire.reset_search();                            // Reset search
  oneWire.search(tempSensor);                        // Search for temp sensor and assign address to tempSensor
  //-----------------------------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------------------------
  // DallasTemperature Library initialisation
  //-----------------------------------------------------------------------------------------------------------  
  sensors.begin();                                   // Initialise 1-wire bus
  //-----------------------------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------------------------
  // Temperature sensor initialisation
  //-----------------------------------------------------------------------------------------------------------
  if (sensors.getDeviceCount() == 0) {
    tempSensorPresent = false;                       // temperature sensor not installed
  } else {
    tempSensorPresent = true;                               // temperature sensor installed - set it up and get initial value
    sensors.setResolution(tempSensor, TEMP_RESOLUTION);     // Set the resolution
    sensors.requestTemperatures();                          // Get the Temperatures
    currentTemperature = getTemperature();                  // Save current temperature
  }
  //-----------------------------------------------------------------------------------------------------------

  //-----------------------------------------------------------------------------------------------------------
  // HC05 Config if required
  //-----------------------------------------------------------------------------------------------------------
  #ifdef BLUETOOTH_COMMS
    HC05Config();
  #endif
  //-----------------------------------------------------------------------------------------------------------
  
}
//=============================================================================================================



//=============================================================================================================
// Main Loop
//=============================================================================================================

void loop()
{
  // If using Bluetooth call doSerialEvent explicitly to check for incoming commands
  // If using USB Serial it has built-in serialEvent which is called automatically between every loop()
  #ifdef BLUETOOTH_COMMS
    BTSerialEvent();
  #endif
  
  // Process the command string if complete
  if (stringComplete)
    doCommand(inputString);

  // If any motor movement required do it
  doMovement();
}
//=============================================================================================================

//-----------------------------------------------------------------------------------------------------------
// HC05 Config
//-----------------------------------------------------------------------------------------------------------
void HC05Config() {
  #ifdef BLUETOOTH_COMMS
    SERIALPORT.begin(38400);                    // switch to 38400 for AT Mode
    delay(500);

    // Put HC05 in AT Command Mode
    digitalWrite(BTSERIAL_PWR_PIN, HIGH);       // Power OFF
    delay(250);
    digitalWrite(BTSERIAL_MODE_PIN, HIGH);      // EN High
    delay(250);
    digitalWrite(BTSERIAL_PWR_PIN, LOW);        // Power ON
  
    delay(2000);

    // Configure Parameters
    SERIALPORT.print("AT+NAME="+BT_NAME+"\n\r");      // Set Module Name
    delay(2000);
    SERIALPORT.print("AT+PSWD="+BT_PWD+"\n\r");       // Set Module Password
    delay(2000);

    // Take HC05 out of AT mode and into comms mode
    digitalWrite(BTSERIAL_PWR_PIN, HIGH);       // Power OFF
    delay(250);
    digitalWrite(BTSERIAL_MODE_PIN, LOW);       // EN LOW
    delay(250);
    digitalWrite(BTSERIAL_PWR_PIN, LOW);        // Power ON

    SERIALPORT.begin(9600);                     // switch back to 9600 for normal comms
  #endif
}
  //-----------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Built-in serialEvent occurs whenever new data comes in the Hardware Serial
// This is called automatically between each loop() run
//-------------------------------------------------------------------------------
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if incoming character is hash, set flag so main loop can action it
    if (inChar == '#') {
      stringComplete = true;
    } 
  }
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// BTSerialEvent checks for incoming on Bluetooth - called explicitly from loop()
//-------------------------------------------------------------------------------
void BTSerialEvent() {
  while (SERIALPORT.available()) {
    char inChar = (char)SERIALPORT.read();   // get the new byte
    inputString += inChar;                 // add it to inputString

    // if incoming character is hash, set flag so doCommand can action
    if (inChar == '#')
      stringComplete = true;
  }
}
//-------------------------------------------------------------------------------


//------------------------------------------------------------------
// Move the position by a single step if movement needed
//------------------------------------------------------------------
void doMovement() {
  if (targetPosition != currentPosition) {
    isMoving = true;
    
    // Adjust speed according to distance yet to travel
    int distance = currentPosition - targetPosition;
    if (abs(distance) > MOTOR_SPEED_THRESHOLD) {
      motorSpeed = MOTOR_SPEED_HI;
    } else {
      motorSpeed = MOTOR_SPEED_LO;
    }
    
    // Going Anticlockwise to lower position
    if (targetPosition < currentPosition) {
      for (int i=0; i < MOTOR_STEPS_PER_DRIVER_STEP; i++)
        anticlockwise();
      
      currentPosition--;
    }
    
    // Going Clockwise to higher position
    if (targetPosition > currentPosition) {
      for (int i=0; i < MOTOR_STEPS_PER_DRIVER_STEP; i++)
        clockwise();
      
      currentPosition++;
    }
       
  } else {
    clearOutput();
    isMoving = false;
    
    // save new position in EEPROM if it has changed
    if (currentPosition != lastSavedPosition) {
      savePosition(currentPosition);
      lastSavedPosition = currentPosition;
    }
    
  }
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Move stepper anticlockwise 
//------------------------------------------------------------------
void anticlockwise()
{
  step++;
  if (step > 7)
    step = 0;

  setOutput(step);
  delayMicroseconds(motorSpeed);
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Move stepper clockwise 
//------------------------------------------------------------------
void clockwise()
{
  step--;
  if (step < 0)
    step = 7;
    
  setOutput(step);
  delayMicroseconds(motorSpeed);
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Set output pins for stepper
//------------------------------------------------------------------
void setOutput(int out)
{
  for (int i=0; i<4; i++)
    digitalWrite(motorPins[i], bitRead(stepPattern[out], i));
  
  outputActive = true;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Clear output pins for stepper
// To ensure they are not left in an on state after movement
//------------------------------------------------------------------
void clearOutput()
{
  if (outputActive) {               // only clear if already active
    for (int i=0; i<4; i++) 
      digitalWrite(motorPins[i], 0);
    
    outputActive = false;
  }
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Get Temperature 
//------------------------------------------------------------------
double getTemperature()
{
  sensors.requestTemperatures();                           // Get temperatures
  double tempC = sensors.getTempC(tempSensor);             // Get Temperature from our (single) Sensor
  
  if (tempC != -127.00)           // if -127 then error so ignore
    currentTemperature = tempC;
  
  return currentTemperature;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Save position in EEPROM - split into 2 byte values. Also sets position valid
//------------------------------------------------------------------
void savePosition(unsigned int p)
{
  byte lowByte = ((p >> 0) & 0xFF);
  byte highByte = ((p >> 8) & 0xFF);
  
  EEPROM.write(EE_LOC_POS, lowByte);
  EEPROM.write(EE_LOC_POS + 1, highByte);
  
  EEPROM.write(EE_LOC_PSTAT, POS_VALID);   // stored position is valid
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Restore position from EEPROM
//------------------------------------------------------------------
unsigned int restorePosition(void)
{
  byte lowByte = EEPROM.read(EE_LOC_POS);
  byte highByte = EEPROM.read(EE_LOC_POS + 1);
  
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Check if stored position in EEPROM is valid
//------------------------------------------------------------------
boolean storedPositionValid(void)
{
  byte status = EEPROM.read(EE_LOC_PSTAT);
  if (status == POS_VALID)
    return true;
  else
    return false;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Do commands incoming from SERIALPORT
//------------------------------------------------------------------
void doCommand(String command) {

  switch(command.charAt(0)) {
    case '#':  // Confirm Connection
      SERIALPORT.print("OK!#");
      break;
    case 'T': // Set Target Position
      {
        int hashpos = command.indexOf('#');    // position of hash in string
        String targetPosS = command.substring(1,hashpos);
        unsigned int targetPosI = targetPosS.toInt();
        targetPosition = targetPosI;
        SERIALPORT.print("T" + targetPosS + ":OK#");
        break;
      }
    case 'C': // Get Temperature
      {
        double t;
        
        // if moving block temperature requests as they interfere with movement. Just return last reading.
        if (isMoving)
          t = currentTemperature;
        else
          t = getTemperature(); 
        
        SERIALPORT.print("C");
        SERIALPORT.print(t * 100, 0);
        SERIALPORT.print(":OK#");
        break;
      }
    case 'I': // Set Initial Position. Sets Position without any movement
      {
        int hashpos = command.indexOf('#');    // position of hash in string
        String initPosS = command.substring(1,hashpos);
        unsigned int initPosI = initPosS.toInt();
        currentPosition = initPosI;
        targetPosition = initPosI;
        SERIALPORT.print("I" + initPosS + ":OK#");
        break;
      }
    case 'P': // Get Current Position
      {
        String currentPositionS = String(currentPosition);
        SERIALPORT.print("P" + currentPositionS + ":OK#");
        break;
      }
    case 'H': // Halt
      {
        targetPosition = currentPosition;
        String currentPositionS = String(currentPosition);
        SERIALPORT.print("H" + currentPositionS + ":OK#");
        break;
      }
    case 'M': // Is motor moving
      {
        if (isMoving) {
          SERIALPORT.print("M1:OK#");
        } else {
          SERIALPORT.print("M0:OK#");
        }
        break;
      }
    case 'V': // Get Version and abilities
      {
        String tempInstalled = (tempSensorPresent ? " | Temp. Sensor |" : "");
        String posValid = (storedPositionValid() ? " | Stored Position |" : "");
        
        SERIALPORT.print( programName + " V" + programVersion + tempInstalled + posValid + "#");
        break;
      }
    default:
      {
        motorSpeed = MOTOR_SPEED_DEFAULT;
        SERIALPORT.print("ERR#");
        break;
      }
  }

  // clear the command string and flag
  inputString = "";
  stringComplete = false;
}
//------------------------------------------------------------------


