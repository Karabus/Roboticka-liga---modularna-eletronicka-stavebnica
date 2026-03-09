#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire1);
static imu::Vector<3> acc;
void setup() {
  Serial.begin(115200);
  while(!Serial);

  
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();

  if (!bno.begin()) {
    Serial.println("BNO055 not found");
    while (1);
  }
  bno.setExtCrystalUse(true);
  
  Serial.println("BNO055 OK");
}

void loop() {
  acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  Serial.print("Acceleration : X: ");
  Serial.print(acc.x());
  Serial.print(" Y: ");
  Serial.print(acc.y());
  Serial.print(" Z: ");
  Serial.println(acc.z());

  delay(BNO055_SAMPLERATE_DELAY_MS);
}