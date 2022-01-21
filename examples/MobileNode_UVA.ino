/* UV A light sensor example sketch
/  This sketch reads the UVA light sensor value.
/  Integration Times and UVA Sensitivity:
/    Rset=270k -> 1T=112.5ms ->   5.625 uW/cm^2/step
/  http://ciudadoscura.com
*/

#include <Wire.h>

#define I2C_ADDR 0x38 // VEML6070 IIC address

//Integration Time
#define IT_1   0x1 //1T

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write((IT_1<<2) | 0x02);
  Wire.endTransmission();
  delay(500);
}

void loop()
{
  byte msb=0, lsb=0;
  uint16_t uv;

  Wire.requestFrom(I2C_ADDR+1, 1); //MSB
  delay(1);
  if(Wire.available())
    msb = Wire.read();

  Wire.requestFrom(I2C_ADDR+0, 1); //LSB
  delay(1);
  if(Wire.available())
    lsb = Wire.read();

  uv = (msb<<8) | lsb;
  Serial.print("UVA: ");
  Serial.println(uv, DEC); //output in steps (16bit)

  delay(1000);
}
