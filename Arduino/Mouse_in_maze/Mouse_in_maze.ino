#include <array>
#include <algorithm>
#include "globals.h"
#include "motion.h"
#include "mouse.h"
#include "maze_map.h"

// Definície globálnych premenných (extern je v globals.h)
Map          mapa;
Cell*        root         = nullptr;
uint8_t      curDirection = 0;
uint8_t      curX         = 0;
uint8_t      curY         = 0;

static imu::Vector<3> rotations;
static SensorData d = {0, 0, 0, 0.0f, 0};

void setup() {
  setupI2C();
  setupServos();
  setupButton();
  Serial.begin(115200);
  while (!Serial) delay(10);

  mapa.begin(2, 2);
  root = &mapa.at(0, 0);

  if (!setupGyroscope()) { Serial.println("FATAL: Gyroskop!"); while (1) {} }
  if (!setupLasers())    { Serial.println("FATAL: Lasery!");   while (1) {} }

  Serial.println("System pripraveny. Stlac ENTER pre start...");
  while (!Serial.available()) {}
  Serial.read();
}

void loop() {
  SensorData d = readSensors();
  Serial.print(d.left);
  Serial.print(" | ");
  Serial.print(d.front);
  Serial.print(" | ");
  Serial.print(d.right);
  Serial.print(" | heading: ");
  Serial.println(d.heading);
  delay(100);
}