#include <Wire.h>
#include <VL53L0X.h>

const int xshutPins[3] = {0, 1, 2};
const uint8_t addresses[3] = {0x30, 0x31, 0x32};
bool good = true;
VL53L0X sensors[3];
TwoWire &BUS = Wire;

void setup() {
  Serial.begin(115200);
  delay(2000);
  BUS.setSDA(4);
  BUS.setSCL(5);
  BUS.begin();

  // všetky senzory OFF
  for (int i = 0; i < 3; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(10);

  // zapínanie po jednom + nastavenie adresy
  for (int i = 0; i < 3; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(50);

    sensors[i].setBus(&BUS);
    if (!sensors[i].init()) {
      Serial.print("VL53 FAIL #"); Serial.println(i);
      good = false;
    }

    sensors[i].setAddress(addresses[i]);

    Serial.print("VL53 #"); Serial.print(i);
    Serial.print(" OK @ 0x");
    Serial.println(addresses[i], HEX);
  }

  for (int i = 0; i < 3; i++) {
    sensors[i].startContinuous(50);
  }
}
void loop() {
  if (!good){
    Serial.println("Nastala chyba pri inicializovani.");
    while(1);
  }
  else{
    
    int left  = sensors[0].readRangeContinuousMillimeters();
    int front = sensors[1].readRangeContinuousMillimeters();
    int right = sensors[2].readRangeContinuousMillimeters();

    Serial.print(left);
    Serial.print(" | ");
    Serial.print(front);
    Serial.print(" | ");
    Serial.println(right);

    delay(50);
  }
  
}