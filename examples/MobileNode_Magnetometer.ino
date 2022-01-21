/* Magnetometer example sketch
/  This sketch reads BMC150 magnetometer part X axis value.
/  http://ciudadoscura.com
*/
#include "Wire.h"
#define magAddress 0x12 // BMC150 magnetometer part IIC address.
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(magAddress);
  Wire.write(0x4B);
  Wire.write(0x01); // Write 0x01 to 0x4B register.
  Wire.endTransmission();
  delay(2);
  Wire.beginTransmission(magAddress);
  Wire.write(0x4C);
  Wire.write(0x30); // Write 0x30 to 0x4C register.
  Wire.endTransmission();
}
 
void getMagData(byte *lsb,byte *msb)
{
  Wire.beginTransmission(magAddress);
  Wire.write(0x42); //
  Wire.endTransmission();
  Wire.requestFrom(magAddress, 2); // Read 2 bytes (0x42 and 0x43).
  *lsb = Wire.read();
  *msb = Wire.read();
}
 
void showMagData()
{
  byte lsb,msb;
  int res;
  getMagData(&lsb,&msb);
  res = ((msb<<5)|(lsb>>3))<<3;
  Serial.println(res/8); // Remove 3 LSB zeros.
  delay(50);
}
 
void loop()
{
  showMagData();
}
