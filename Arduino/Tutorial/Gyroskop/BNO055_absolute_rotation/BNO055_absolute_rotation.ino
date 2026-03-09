#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire1);
static imu::Vector<3> euler;

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
  bno.setExtCrystalUse(true);

  Serial.println("BNO055 OK");
}

void loop() {
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  float yaw = euler.x();    // otocenie okolo Z (doprava/dolava) 0–360°
  float roll = euler.z();   // naklonenie do stran  -180-180°
  float pitch = euler.y();  // naklonenie dopredu/dozadu -180-180°

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("  Pitch: ");
  Serial.print(pitch);
  Serial.print("  Yaw: ");
  Serial.println(yaw);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}