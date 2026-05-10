#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
#include <Servo.h>
int adresa = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
static VL53L0X sensors[3];
bool active = LOW;
const int laserPins[3] = {0,2,4}; // < ^ >

class Node{
  public:           // <    ^     >     v
    Node *neigh[4] = {NULL, NULL, NULL, NULL};
    void addNeigh(int i, Node* node){
      if (0<= i && i<=3){neigh[i]=node;}
    }
};
Node root;

int setupLasers(){ // 0 = <, 2 = ^, 4 = >
  Serial.println("VL53L0X test...");
  for (int i = 0; i < 3;i++){
    Wire.setSDA(laserPins[i]);  
    Wire.setSCL(laserPins[i]+1); 
    Wire.begin();

    if (!sensors[i].init()) {
      Serial.println("Sensor " + String(i) +" nenajdeny!");
      return -1;
    }
  }
  Serial.println("Sensors OK");
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
  if (init!=0){
    while(1){}
  }
  init = setupLasers();

}

void loop() {
  

}
