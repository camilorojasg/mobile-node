/* Pulse LED example sketch
/  This sketch makes blink the MobileNode user LED.
/  http://ciudadoscura.com
*/
int led = 5;

void setup() {
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
}

void loop() {
  for(int i = 0; i < 255; i++){
    analogWrite(led,i);
    delay(2);
  }
  
  for(int i = 254; i >0; i--){
    analogWrite(led,i);
    delay(2);
  }
}
