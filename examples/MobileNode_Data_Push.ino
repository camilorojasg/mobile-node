#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <Math.h>

//Data service public and private keys
const char public_key[] = "PUT YOUR KEY";
const char private_key[] = "PUT YOUR KEY";

//APN configuration
const char apn_server[] = "PUT YOUR APN SERVER";
const char apn_user[] = "PUT YOUR APN USER";
const char apn_password[] = "PUT YOUR APN PASSWORD";

//Magnetometer calibration offsets
const double xoffset = 0;
const double yoffset = 0;

//UV Index calculation constant
const float uv_const = 0.0225;

#define UV_ADDR 0x38 // VEML6070 IIC address
#define IT_1 0x0 //VEML6070 integration time
#define MAG_ADDR 0x12 //BMC150 magnetometer IIC address

TinyGPS gps;
SoftwareSerial gpsSerial(9, 8); // RX, TX

String url;
int gpsPower = 4;
int gsmPower = 11;
int led = 5;
int gpio = 6;
int ain = 2;

void setup() {
  //VEML6070 configuration
  Wire.begin();
  Wire.beginTransmission(UV_ADDR);
  Wire.write((IT_1<<2) | 0x02);
  Wire.endTransmission();
  delay(20);
  
  //Magnetometer configuration
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x4B);
  Wire.write(0x01); // Write 0x01 to 0x4B register.
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x4C);
  Wire.write(0x30); // Write 0x30 to 0x4C register.
  Wire.endTransmission();
  
  delay(500);
  Serial.begin(9600);
  Serial1.begin(9600); // GSM/GPRS module
  gpsSerial.begin(9600);
  pinMode(gpsPower, OUTPUT);
  pinMode(gsmPower, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(gpio, INPUT);
  digitalWrite(gpsPower, LOW);
  digitalWrite(gpsPower, LOW);
  digitalWrite(gsmPower, LOW);
  
  toggleGSMPower();
  delay(20000);
}

void toggleGSMPower(){
  digitalWrite(gsmPower, HIGH);
  delay(1000);
  digitalWrite(gsmPower, LOW);
}

void blinkLed(){
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}

void printUV(){
  byte msb=0, lsb=0;
  uint16_t uv;

  Wire.requestFrom(UV_ADDR+1, 1); //MSB
  delay(1);
  if(Wire.available())
    msb = Wire.read();

  Wire.requestFrom(UV_ADDR+0, 1); //LSB
  delay(1);
  if(Wire.available())
    lsb = Wire.read();

  uv = (msb<<8) | lsb;
  int uvindexint = uv * uv_const;
  url = String(url + String(uvindexint, DEC));
}

void getxMagData(byte *lsb,byte *msb)
{
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x42); //
  Wire.endTransmission();
  Wire.requestFrom(MAG_ADDR, 2); // Read 2 bytes (0x42 and 0x43).
  *lsb = Wire.read();
  *msb = Wire.read();
}
 
void getyMagData(byte *lsb,byte *msb)
{
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x44); //
  Wire.endTransmission();
  Wire.requestFrom(MAG_ADDR, 2); // Read 2 bytes (0x42 and 0x43).
  *lsb = Wire.read();
  *msb = Wire.read();
}

void printHeading(){
  byte ylsb,ymsb, xlsb, xmsb;
  int yres, xres;
  getyMagData(&ylsb,&ymsb);
  getxMagData(&xlsb,&xmsb);
  yres = ((ymsb<<5)|(ylsb>>3))<<3;
  xres = ((xmsb<<5)|(xlsb>>3))<<3;
  yres /= 8;
  xres /= 8;
  yres += yoffset;
  xres += xoffset;
  int heading = 360 - (atan2((double)yres,(double)-xres) * (180 / 3.14159265) + 180);
  url = String(url + String(heading, DEC));
}

void activateAPN(){
  Serial1.print("AT+QIREGAPP=\"");
  Serial1.print(apn_server);
  Serial1.print("\",\"");
  Serial1.print(apn_user);
  Serial1.print("\",\"");
  Serial1.print(apn_password);
  Serial1.println("\"");
  delay(1000);
  Serial1.println("AT+QIACT");
  delay(1000);
}

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);

bool checkFix(unsigned long fix_age){
  if(fix_age == TinyGPS::GPS_INVALID_AGE || fix_age > 5000){
    return false;
  }else{
    return true;
  }
}

String batteryCharge(){
  String battery = "0";
  //Empty Serial buffer
  while(Serial1.available()){
    Serial1.read();
  }
  //Issue command
  Serial1.println("AT+CBC");
  delay(300);
  //Read battery charge percentage
  if(Serial1.available()){
      Serial1.readStringUntil(',');
      battery = Serial1.readStringUntil(',');
      while(Serial1.available()){
        Serial1.read();
      }
  }
  return battery;
}

void loop()
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;

  gps.f_get_position(&flat, &flon, &age);
  if(checkFix(age)){
  activateAPN();
  url = String("&battery=" + batteryCharge());
  url = String(url + "&latitude=");
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  url = String(url + "&longitude=");
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  url = String(url + "&azimuth=");
  printHeading();
  url = String(url + "&ain=");
  url = String(url + String(analogRead(ain), DEC));
  url = String(url + "&gpio=");
  url = String(url + String(digitalRead(gpio), DEC));
  url = String(url + "&uv=");
  printUV();
  Serial1.print("AT+QHTTPURL=");
  Serial1.print(url.length()+102, DEC);
  Serial1.println(",1");
  delay(100);
  Serial1.print("http://data.ciudadoscura.com/input/");
  Serial1.print(public_key);
  Serial1.print("?private_key=");
  Serial1.print(private_key);
  Serial1.println(url);
  delay(100);
  url = String("");
  Serial1.println("AT+QHTTPGET=10");
  delay(3000);
  while(Serial1.available()){
    Serial1.read();
  }
  Serial1.println("AT+QHTTPREAD=2");
  delay(2000);
  while(Serial1.available()){
    if(Serial1.read()=='1'){
      blinkLed();
      Serial.println("Success!");
    }
  }
  Serial1.println("AT+QIDEACT");
  delay(2000);
  }else{
    Serial.println("Invalid GPS coordinates.");
  }
  
  smartdelay(1000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      url = String(url + '0');
  }
  else
  {
    url = String(url + String(val, prec));
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;

  }
  smartdelay(0);
}


