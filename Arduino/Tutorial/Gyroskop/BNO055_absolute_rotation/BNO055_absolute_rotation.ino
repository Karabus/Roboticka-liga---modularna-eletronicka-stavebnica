#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.setSDA(2);
  Wire.setSCL(3);
  Wire.begin();

  Serial.println("Init BNO055...");

  if (!bno.begin()) {
    Serial.println("BNO055 nenajdeny");
    return;
  }

  bno.setExtCrystalUse(true);
  Serial.println("BNO055 OK");
}

void loop() {
  
  Serial.println("BNO055 OK");
  /*imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  Serial.print("Yaw: ");
  Serial.print(euler.x());
  Serial.print(" Roll: ");
  Serial.print(euler.z());
  Serial.print(" Pitch: ");
  Serial.println(euler.y());
*/
  delay(100);
}