#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
int adresa = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
bool active = LOW;
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

  if (adresa > 0) {
    bno = Adafruit_BNO055(55, adresa, &Wire);
    if (bno.begin()) {
      active = HIGH;
      Serial.println("BNO055 OK");
    } 
    else {
      Serial.println("BNO055 begin() zlyhalo");
    }
  }
}

void loop() {
  if (!active) {
    Serial.println("BNO055 not OK");
    delay(1000);
    return;  // zastav loop ak senzor nefunguje
  }
  uint8_t system, gyro, accel, mag;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.print("Kalibracia: Sys="); Serial.print(system);
  Serial.print(" Gyro="); Serial.print(gyro);
  Serial.print(" Accel="); Serial.print(accel);
  Serial.print(" Mag="); Serial.println(mag);

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  Serial.print("X: "); Serial.print(euler.x());
  Serial.print("  Y: "); Serial.print(euler.y());
  Serial.print("  Z: "); Serial.println(euler.z());
  delay(100);
}