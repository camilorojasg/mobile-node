/* Accelerometer example sketch
/  This sketch reads BMC150 accelerometer part Y axis value.
/  http://ciudadoscura.com
*/
#include "Wire.h"
#define accAddress 0x10 // BMC150 accelerometer part IIC address.
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(accAddress);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);
}
 
void getAxisAcc(char registerNum, byte *lsb, byte *msb)
{
  Wire.beginTransmission(accAddress);
  Wire.write(registerNum);
  Wire.endTransmission();
  Wire.requestFrom(accAddress, 2);
  *lsb = Wire.read();
  *msb = Wire.read();
}
 
void showAccData()
{
  byte ylsb, ymsb;
  int yres;
  getAxisAcc(0x04,&ylsb,&ymsb);
  yres = ((ymsb<<4)|(ylsb>>4))<<4;
  Serial.print("Y axis: ");
  Serial.println(yres/16); // Remove 4 LSB zeros.
  delay(50);
}
 
void loop()
{
  showAccData();
}
