#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire1);

void setup() {
  Serial.begin(115200);
  delay(1500);

  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();

  if (!bno.begin()) {
    Serial.println("BNO055 nenajdeny");
    return;
  }

  bno.setExtCrystalUse(true);

  Serial.println("BNO055 OK");
  Serial.println("SYS GYRO ACC MAG");
}

void loop() {

  uint8_t sys, gyro, accel, mag;

  bno.getCalibration(&sys, &gyro, &accel, &mag);

  Serial.print("CALIBRATION: ");
  Serial.print(sys);
  Serial.print(" ");
  Serial.print(gyro);
  Serial.print(" ");
  Serial.print(accel);
  Serial.print(" ");
  Serial.println(mag);
  /*Ako kalibrovat
  Gyroscope - nechaj senzor 5–10 sekund uplne bez pohybu
  Accelerometer -n aklanaj senzor do roznych smerov
  Magnetometer - rob pohyb osmičky vo vzduchu
  System - kalibruje sa automaticky ked su ostatne hotove*/
  delay(100);
}