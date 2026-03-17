#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Nastavenie I2C pinov (tvoje ADC piny)
  Wire.setSDA(29);  // SDA = GP29 (ADC3)
  Wire.setSCL(28);  // SCL = GP28 (ADC4)
  Wire.begin();

  Serial.println("VL53L0X test...");

  if (!lox.begin()) {
    Serial.println("Sensor nenajdeny!");
    while (1);
  }

  Serial.println("Sensor OK");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    Serial.print("Vzdialenost: ");
    Serial.print(measure.RangeMilliMeter);
    Serial.println(" mm");
  } else {
    Serial.println("Out of range");
  }

  delay(500);
}