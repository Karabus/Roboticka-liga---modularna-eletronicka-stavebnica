#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);
static imu::Vector<3> euler;

void setup() {
  Serial.begin(115200);
  delay(1500); // NAMIesto while(!Serial)

  // nastav I2C piny
  Wire.setSDA(1);
  Wire.setSCL(2);
  Wire.begin();

  if (!bno.begin()) {
    Serial.println("BNO055 nenajdeny");
    return; // NIKDY nepouzivaj while(1)
  }

  bno.setExtCrystalUse(true);
  Serial.println("BNO055 OK");
}

void loop() {
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  float yaw = euler.x();
  float roll = euler.z();
  float pitch = euler.y();

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("  Pitch: ");
  Serial.print(pitch);
  Serial.print("  Yaw: ");
  Serial.println(yaw);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}