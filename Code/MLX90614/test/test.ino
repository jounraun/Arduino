#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <OneWire.h>
#include <DallasTemperature.h>


Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {

  Serial.begin(9600);

  mlx.begin();

  delay(1000);
}

void loop() {

  mlx_sensor();

  delay(1000);
}

void mlx_sensor()
{

  String str;

  str = String("Ambient: ") + String(mlx.readAmbientTempC());
  Serial.println(str);

  str = String("Object: ") + String(mlx.readObjectTempC());
  Serial.println(str);
}

