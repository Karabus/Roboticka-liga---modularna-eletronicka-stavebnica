#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);
  Wire.setSDA(4);  // GP4
  Wire.setSCL(5);  // GP5
  Wire.begin();

  if (!sensor.init()) {
    Serial.println("Sensor nenajdeny!");
    while (1);
  }

  sensor.setTimeout(500);
  sensor.startContinuous();
  Serial.println("VL53L0X OK");
}

void loop() {
  uint16_t dist = sensor.readRangeContinuousMillimeters();

  if (sensor.timeoutOccurred()) {
    Serial.println("Timeout!");
  } else {
    Serial.print("Vzdialenost: ");
    Serial.print(dist);
    Serial.println(" mm");
  }

  delay(100);
}