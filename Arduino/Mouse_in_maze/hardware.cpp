#include "hardware.h"
// ============================================================
// SETUP GYROSKOP
// ============================================================
Adafruit_BNO055 setupGyroscope() {
  Wire1.setSDA(BNO_SDA);
  Wire1.setSCL(BNO_SCL);
  Wire1.begin();
  delay(100);

  Serial.println("Hladam BNO055...");
  int foundAddr = -1;
  for (byte addr = 1; addr < 127; addr++) {
    Wire1.beginTransmission(addr);
    if (Wire1.endTransmission() == 0) {
      Serial.print("Najdene na adrese: 0x");
      Serial.println(addr, HEX);
      foundAddr = addr;
      break;
    }
  }

  if (foundAddr < 0) { Serial.println("BNO055: nenajdene!"); return nullptr; }
  bno = Adafruit_BNO055(55, foundAddr, &Wire1);
  if (!bno.begin()) { Serial.println("BNO055 begin() zlyhalo!"); return nullptr; }

  Serial.println("BNO055 OK");
  return bno;
}

// ============================================================
// SETUP VL53L0X
// ============================================================
int setupLasers() {
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();

  for (int i = 0; i < 3; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(10);

  for (int i = 0; i < 3; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(10);
    sensors[i].setBus(&Wire);
    if (!sensors[i].init()) {
      Serial.print("VL53L0X #"); Serial.print(i); Serial.println(" nenajdeny!");
      return -1;
    }
    sensors[i].setAddress(sensorAddrs[i]);
    Serial.print("VL53L0X #"); Serial.print(i);
    Serial.print(" OK → 0x"); Serial.println(sensorAddrs[i], HEX);
  }

  for (int i = 0; i < 3; i++) sensors[i].startContinuous(50);
  Serial.println("Vsetky senzory OK");
  return 0;
}

// ============================================================
// SENZORY
// ============================================================
SensorData readSensors() {
  SensorData data;
  data.left  = sensors[0].readRangeContinuousMillimeters();
  data.front = sensors[1].readRangeContinuousMillimeters();
  data.right = sensors[2].readRangeContinuousMillimeters();

  if (gyroActive) {
    sensors_event_t event;
    bno.getEvent(&event);
    data.heading = event.orientation.x;
    data.dir = (int)(data.heading / 90.0f) % 4;
  } else {
    data.heading = -1;
    data.dir = 0;
  }
  return data;
}