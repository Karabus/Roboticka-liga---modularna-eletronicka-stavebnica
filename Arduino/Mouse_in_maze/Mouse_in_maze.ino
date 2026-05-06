#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
int adresa = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
VL53L0X sensor;

bool active = LOW;

const int laserPins[3]] = {0,2,4} // < ^ >

int setupLaser(int dir){ // 0 = <, 2 = ^, 4 = >
  Wire.setSDA(laserPins[dir]);  
  Wire.setSCL(laserPins[dir]+1); 
  Wire.begin();
  

  Serial.println("VL53L0X test...");

  if (!lox.begin()) {
    Serial.println("Sensor nenajdeny!");
    return -1;
  }

  Serial.println("Sensor OK");
  return 0;
}

int setupGyroscope(){
  Wire1.setSDA(4);
  Wire1.setSCL(5);
  Wire1.begin();
  delay(1000);

  Serial.println("Hladam I2C zariadenia...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire1.beginTransmission(addr);
    if (Wire1.endTransmission() == 0) {
      Serial.print("Najdene zariadenie na adrese: 0x");
      Serial.println(addr, HEX);
      adresa = addr;
      break;
    }
  }

  if (adresa > 0) {
    bno = Adafruit_BNO055(55, adresa, &Wire1);
    if (bno.begin()) {
      active = HIGH;
      Serial.println("BNO055 OK");
      return 0;
    } 
    Serial.println("BNO055 begin() zlyhalo");
    return -1;
  }
  Serial.println("Nenasla sa adresa");
  return -2;
}

void setup() {
  // put your setup code here, to run once:
  int init = setupGyroscope();
  if (!init){
    while(1){}
  }
  init = 
}

void loop() {
  // put your main code here, to run repeatedly:

}
