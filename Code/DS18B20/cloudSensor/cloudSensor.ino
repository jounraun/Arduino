/*
 *  cloudSensor control software 
 *
 *  Reads data from MLX90614 IR thermometer and one DS1820 series thermistor 
 *  if requested via Serial (letter 'r' is sent to Arduino). The values are 
 *  printed to Serial as CSV values in Celsius.
 *
 *  MLX90614 code and i2cmaster.h are from
 *  http://bildr.org/2011/02/mlx90614-arduino/
 *  
 *  DS1820 code is from the DallasTemperature examples.
 *
 *  The rest is by Panu Lahtinen, December 8, 2011.
 * 
 *  Use as is, modify if need be, redistribute as free software.
 *  
 *  For licences, please refer to Blidr page for their licence and respective 
 *  library documentation of their licences.
 *
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <i2cmaster.h>

#define ONE_WIRE_BUS 3  
#define TEMPERATURE_PRECISION 9

#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Sensor address
DeviceAddress T;

void setup(){
  Serial.begin(9600);
  delay(250);

Serial.println("hello1");

  i2c_init();
Serial.println("hello2");

  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups 

  // Start the Dallas temperature sensor library
  sensors.begin();
Serial.println("hello3");

  // Read the address of first DS1820 sensor
  sensors.getAddress(T,0);
Serial.println("hello4");

  // Set temperature resolution
  sensors.setResolution(T, TEMPERATURE_PRECISION);
Serial.println("hello5");

  delay(100);

}

void loop(){
  
Serial.println("hello6");

  // Wait for letter 'r' from the Serial interface
//  if(Serial.read() == 'r'){
    // Request temperature from Dallas sensor(s)
    sensors.requestTemperatures();
Serial.println("hello7");

    delay(500);
    // Read temperature from MLX90614 IR thermometer
    float t_ir = get_mlx_temp();
Serial.println("hello8");

    // Read the ambient (air) temperature
    float t = sensors.getTempC(T);
    // Print the valus to Serial
    Serial.print(t); 
    Serial.print(",");
    Serial.print(t_ir);

    int chk;
//  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
//                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // DISPLAT DATA
  Serial.print(",");
  Serial.print(DHT.temperature,1);
  Serial.print(",");
  Serial.println(DHT.humidity,1);

  delay(1000);

//  }   
}


/* Read temperature from Melexis IR temperature sensor
 */
float get_mlx_temp(){
  //Address must be bit shifted left, library does not automatically do it
  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  i2c_start_wait(dev+I2C_WRITE); //send start condition and write bit
  i2c_write(0x07); //send command for device to action

  i2c_rep_start(dev+I2C_READ); //send repeated start condition, device will ack
  data_low = i2c_readAck();  //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak(); //Read error check byte and send Nack to tell device no more data to send
  i2c_stop(); //Release bus, end transaction

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB
  int tempData = 0x0000;

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (int)(((data_high & 0x007F) << 8) + data_low);
  float tempData2 = (tempData * tempFactor)-0.01;
  tempData2 = tempData2 - 273.15;

  return tempData2;
}

