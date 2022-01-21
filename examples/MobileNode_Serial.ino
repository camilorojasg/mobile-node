/* Serial example sketch
/  This sketch connects the MobileNode USB UART with the GPS module or GSM/GPRS module one.
/  http://ciudadoscura.com
*/

/*
// For GPS module -----------------------------
#include <SoftwareSerial.h>
SoftwareSerial gpsSerial(9, 8); // RX, TX

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  while(!Serial){}
}

void loop() {
  if(Serial.available()){
    gpsSerial.write(Serial.read());
  }
   if(gpsSerial.available()){
    Serial.write(gpsSerial.read());
  } 
}
// ---------------------------------------------
*/

// For GSM/GPRS module -------------------------
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  while(!Serial){}
}

void loop() {
  if(Serial.available()){
    Serial1.write(Serial.read());
  }
   if(Serial1.available()){
    Serial.write(Serial1.read());
  } 
}
// -----------------------------------------
