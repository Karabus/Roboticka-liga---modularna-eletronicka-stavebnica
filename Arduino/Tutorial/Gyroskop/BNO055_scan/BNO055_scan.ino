#include <Wire.h>
int adresa = 0;
void setup() {
  Serial.begin(115200);
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  delay(1000);

  Serial.println("Hladam I2C zariadenia...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Najdene zariadenie na adrese: 0x");
      Serial.println(addr, HEX);
      adresa = addr;
    }
  }
  Serial.println("Hotovo.");
}

void loop() {
  Serial.println(adresa, HEX);
}