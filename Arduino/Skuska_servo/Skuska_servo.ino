#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire1);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();

  if (!bno.begin()) {
    Serial.println("BNO055 nenajdeny");
    while (1);
  }

  delay(1000);
}

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  float yaw = euler.x();    // otocenie okolo Z
  float roll = euler.z();   // naklonenie do stran
  float pitch = euler.y();  // naklonenie dopredu/dozadu

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("  Pitch: ");
  Serial.print(pitch);
  Serial.print("  Yaw: ");
  Serial.println(yaw);

  delay(200);
}