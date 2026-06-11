#include <sys/_stdint.h>
#include "motion.h"


void stopMotors() {
  servoLeft.write(STOP);
  servoRight.write(STOP);
}
void goForward() {
  uint8_t newX = curX + mapDirection[curDirection][0];
  uint8_t newY = curY + mapDirection[curDirection][1];

  // Rozšír mapu ak je nová bunka mimo hraníc
  if (newX > mapa.size_X) mapa.resize(false);  // nový stĺpec
  if (newY > mapa.size_Y) mapa.resize(true);   // nový riadok

  // Otvor stenu medzi aktuálnou a novou bunkou
  mapa.at(curX, curY).connect(curDirection, mapa.at(newX, newY));

  // Fyzický pohyb
  float targetAngle = curDirection * 90.0f;
  unsigned long startTime = millis();
  const unsigned long MOVE_TIME = 800;

  while (millis() - startTime < MOVE_TIME) {
    sensors_event_t event;
    bno.getEvent(&event);
    float currentAngle = event.orientation.x;
    float err = targetAngle - currentAngle;
    if (err >  180) err -= 360;
    if (err < -180) err += 360;
    int correction = constrain((int)(0.3f * err), -15, 15);
    servoLeft.write(STOP + SPEED + correction);
    servoRight.write(STOP - SPEED + correction);
    delay(10);
  }
  stopMotors();
  delay(100);

  // Aktualizuj pozíciu
  curX = (uint8_t)newX;
  curY = (uint8_t)newY;
  root = &mapa.at(curX, curY);
}

// dir=0 → vľavo, dir=1 → vpravo
void turn(bool dir) {
  curDirection = (curDirection + 1 + dir * 2) % 4;
  float targetAngle = curDirection * 90.0f;

  if (dir == 1) {
    servoLeft.write(STOP - SPEED);
    servoRight.write(STOP - SPEED);
  } else {
    servoLeft.write(STOP + SPEED);
    servoRight.write(STOP + SPEED);
  }

  unsigned long timeout = millis() + 3000;
  while (millis() < timeout) {
    sensors_event_t event;
    bno.getEvent(&event);
    float err = targetAngle - event.orientation.x;
    if (err >  180) err -= 360;
    if (err < -180) err += 360;
    if (abs(err) < EPSILON) break;
    delay(10);
  }
  stopMotors();
}

