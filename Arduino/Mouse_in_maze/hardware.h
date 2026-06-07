#ifndef HARDWARE_H
#define HARDWARE_H
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
// === I2C ===
#define I2C_SDA  4
#define I2C_SCL  5
#define BNO_SDA 6
#define BNO_SCL 7

// === Servo piny ===
#define SERVO_LEFT_PIN  10
#define SERVO_RIGHT_PIN 11

// === XSHUT piny pre VL53L0X ===
const int xshutPins[3] = {0, 1, 2}; // vľavo, vpredu, vpravo

struct SensorData {
  int left, front, right;
  float heading;
  int dir;
};

int setupGyroscope();
int setupLasers();

VL53L0X sensors[3];
const uint8_t sensorAddrs[3] = {0x30, 0x31, 0x32};

bool gyroActive = false;
#endif