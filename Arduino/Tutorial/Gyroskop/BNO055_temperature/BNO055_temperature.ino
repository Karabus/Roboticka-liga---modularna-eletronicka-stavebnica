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

  Serial.println("BNO055 OK");
}

void loop() {

  int temp = bno.getTemp();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  delay(1000);
}