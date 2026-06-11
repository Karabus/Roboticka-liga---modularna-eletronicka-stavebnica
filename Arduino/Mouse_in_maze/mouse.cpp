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

// ============================================================
// INIT — zavolaj raz v setup() pred hlavnou slučkou
// ============================================================
void mouseInit() {
  LittleFS.begin();
  if (LittleFS.exists("/maze.txt")) {
    Serial.println("Nacitavam mapu...");
    uint8_t gx = 255, gy = 255;  // 255 = žiadny goal
    if (mapa.loadMap(curX, curY, curDirection, gx, gy)) {
      root = &mapa.at(curX, curY);
      if (gx != 255 && gy != 255) {
        // Python pridal G: → rovno do NAVIGATE
        goalX = gx;
        goalY  = gy;
        currentState = NAVIGATE;
        Serial.print("Goal najdeny → NAVIGATE: ");
        Serial.print(gx); Serial.print(", "); Serial.println(gy);
      } else {
        currentState = IDLE;
        Serial.println("Mapa nacitana → IDLE");
      }
      delay(5000);

    } else {
      Serial.println("Chyba nacitania → EXPLORE");
      mapa.begin(1, 1);
      root = &mapa.at(0, 0);
      currentState = EXPLORE;
    }
  } else {
    Serial.println("Ziadna mapa → EXPLORE");
    delay(2000);
    mapa.begin(1, 1);
    root = &mapa.at(0, 0);
    currentState = EXPLORE;
  }
}

// ============================================================
// STAVOVÝ AUTOMAT — volaj v loop()
// ============================================================
void mouseTick() {
  switch (currentState) {

    // ----------------------------------------------------------
    case START:
      mouseInit();
      break;

    // ----------------------------------------------------------
    case EXPLORE:
      explore();
      if (mapa.fullyExplored()) {
        Serial.println("Bludisko preskumane → ukladam → IDLE");
        mapa.saveMap(curX, curY, curDirection);
        currentState = IDLE;
      }
      break;

    // ----------------------------------------------------------
    case IDLE:
      if (readGoalFromSerial(goalX, goalY)) {
        Serial.print("Ciel: ");
        Serial.print(goalX); Serial.print(", ");
        Serial.println(goalY);
        currentState = NAVIGATE;
      }
      break;

    // ----------------------------------------------------------
    case NAVIGATE:
      if (goToPos(goalX, goalY) == 0) {
        Serial.println("Ciel dosiahnuty → IDLE");
      } else {
        Serial.println("Cesta nenajdena → IDLE");
      }
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