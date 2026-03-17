#include <Wire.h>
#include <Adafruit_VL6180X.h>

Adafruit_VL6180X vl = Adafruit_VL6180X();

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C piny
  Wire.setSDA(29);  // ADC3
  Wire.setSCL(28);  // ADC4
  Wire.begin();

  Serial.println("VL6180X test...");

  if (!vl.begin()) {
    Serial.println("Sensor nenajdeny!");
    while(1);
  }
  Serial.println("Sensor OK");
}

void loop() {
  // Vzdialenosť v mm
  Serial.print("Distance: ");
  Serial.print(vl.readRange());
  Serial.println(" mm");

  // Ambient light (lux)
  Serial.print("Ambient light: ");
  Serial.print(vl.readAmbient());
  Serial.println(" lux");

  delay(500);
}