#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("BOOT");

  Wire.setSDA(4);   // GP4
  Wire.setSCL(5);   // GP5
  Wire.begin();

  Serial.println("I2C begun");

  sensor.setBus(&Wire);
  sensor.setTimeout(500);

  Serial.println("Before init");
  if (!sensor.init()) {
    Serial.println("Sensor FAIL");
    while (1);
  }

  delay(500);  // stabilizácia senzora

  sensor.setTimeout(500);
  sensor.startContinuous(50);

  delay(200);  // warm-up before first read

  Serial.println("VL53L0X OK");
  sensor.startContinuous(50);
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