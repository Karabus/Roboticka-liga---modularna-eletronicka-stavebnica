#ifndef HARDWARE_H
#define HARDWARE_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
#include <Servo.h>

// === I2C piny ===
#define I2C_SDA 4
#define I2C_SCL 5
#define BNO_SDA 6
#define BNO_SCL 7

// === Servo piny ===
#define SERVO_LEFT_PIN  10
#define SERVO_RIGHT_PIN 11

// === XSHUT piny pre VL53L0X ===
// extern — definície sú v hardware.cpp, inak ODR porušenie
extern const int     xshutPins[3];
extern const uint8_t sensorAddrs[3];

struct SensorData {
    int   left, front, right;
    float heading;
    int   dir;
};

extern VL53L0X         sensors[3];
extern Adafruit_BNO055 bno;
extern Servo           servoLeft;
extern Servo           servoRight;
extern bool            gyroActive;

constexpr int   STOP    = 90;
constexpr int   SPEED   = 20;
constexpr float EPSILON = 2.0f;

bool       setupGyroscope();
bool       setupLasers();
void       setupServos();
void       setupI2C();
SensorData readSensors();

#endif