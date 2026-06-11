#include "mouse.h"
#include <LittleFS.h>

const int8_t mapDirection[4][2] = {
  { 1,  0},  // 0 = >
  { 0,  1},  // 1 = ^
  {-1,  0},  // 2 = 
  { 0, -1}   // 3 = v
};

static State currentState = START;
static uint8_t goalX = 0, goalY = 0;

static bool buttonPressed() {
    static bool lastState = HIGH;
    static unsigned long lastDebounce = 0;
    bool state = digitalRead(BUTTON_PIN);
    if (state != lastState) {
        lastDebounce = millis();
        lastState = state;
    }
    if ((millis() - lastDebounce) > 50 && state == LOW) {
        lastDebounce = millis() + 1000;  // blokuj ďalší trigger na 1s
        return true;
    }
    return false;
}

// ============================================================
// INIT — zavolaj raz v setup() pred hlavnou slučkou
// ============================================================
void mouseInit() {
    LittleFS.begin();
    mapa.begin(1, 1);
    root = &mapa.at(0, 0);
    curX = 0; curY = 0; curDirection = 0;
    Serial.println("Pripraveny → cakam na tlacidlo...");
}

// ============================================================
// STAVOVÝ AUTOMAT — volaj v loop()
// ============================================================
void mouseTick() {
  bool btn = buttonPressed();

  switch (currentState) {

    case START:
      if (btn) {
        currentState = IDLE;
        Serial.println("Tlacidlo → IDLE");
      }
      break;

    case EXPLORE:
      if (btn) {
        Serial.println("Tlacidlo → ukladam → IDLE");
        mapa.saveMap(curX, curY, curDirection);
        currentState = IDLE;
        break;
      }
      explore();
      if (mapa.fullyExplored()) {
        Serial.println("Bludisko preskumane → ukladam → IDLE");
        mapa.saveMap(curX, curY, curDirection);
        currentState = IDLE;
      }
      break;

    case IDLE:
      if (btn) {
        uint8_t gx = 255, gy = 255;
        if (mapa.loadMap(curX, curY, curDirection, gx, gy)) {
          root = &mapa.at(curX, curY);
          if (gx != 255 && gy != 255) {
            goalX = gx;
            goalY  = gy;
            Serial.print("Goal nacitany → NAVIGATE: ");
            Serial.print(gx); Serial.print(", "); Serial.println(gy);
            currentState = NAVIGATE;
          } else {
            Serial.println("Ziadny goal → zostavam v IDLE");
            // currentState = IDLE;  ← nič nemeníme
          }
        } else {
          Serial.println("Ziadna mapa → EXPLORE");
          mapa.begin(1, 1);
          root = &mapa.at(0, 0);
          currentState = EXPLORE;
        }
      }
      break;  
    case NAVIGATE:
      if (btn) {
        Serial.println("Tlacidlo → IDLE");
        stopMotors();
        currentState = IDLE;
        break;
      }
      if (goToPos(goalX, goalY) == 0) {
        Serial.println("Ciel dosiahnuty → IDLE");
      } else {
        Serial.println("Cesta nenajdena → IDLE");
      }
      goalX = 255;
      goalY = 255;
      currentState = IDLE;
      break;
  }
}
// ============================================================
// getState
// ============================================================
State getState() {
  return currentState;
}
