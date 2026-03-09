#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();

  Serial.println("I2C scan...");
}

void loop() {
  byte error, address;

  for(address = 1; address < 127; address++ ) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0) {
      Serial.print("I2C device at 0x");
      Serial.println(address, HEX);
    }
  }

  Serial.println("scan done");
  delay(3000);
}