#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
#include <Servo.h>
#include <array>
#include <algorithm>
#include <LittleFS.h>
#include "hardware.h"
#include "maze_map.h"
#include "hardware.h"
//<<<<<<< Updated upstream
static imu::Vector<3> rotations;
//=======
static uint8_t MAZE_H = 1;
static uint8_t MAZE_W = 1;
//>>>>>>> Stashed changes


static SensorData d = {0, 0, 0, 0.0f, 0};


// === Mapa smerov: 0=>  1=^  2=<  3=v ===
const int8_t mapDirection[4][2] = {
  { 1,  0},  // 0 = >
  { 0,  1},  // 1 = ^
  {-1,  0},  // 2 = <
  { 0, -1}   // 3 = v
};

static uint8_t curDirection = 0;
static uint8_t curX = 0;
static uint8_t curY = 0;

const int   STOP    = 90;
const int   SPEED   = 20;
const float EPSILON = 2.0f;

TwoWire &BUS = Wire;

Adafruit_BNO055 bno = nullptr;
Servo servoLeft, servoRight;

// ============================================================
// NODE — graf bludiska
// ============================================================
Map mapa = new Map();
Cell* root = nullptr;

// ============================================================
// POHYB
// ============================================================
void stopMotors() {
  servoLeft.write(STOP);
  servoRight.write(STOP);
}

// dir=1 → vľavo, dir=3 → vpravo
void turn(int8_t dir) {
  curDirection = (curDirection + dir + 4) % 4;
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

void goForward() {
  if (root->neigh[curDirection] != nullptr) {
    root = root->neigh[curDirection];
  } 
  else {
    int newX = root->x + mapDirection[curDirection][0];
    int newY = root->y + mapDirection[curDirection][1];

    if (newX < 0 || newX >= MAZE_W || newY < 0 || newY >= MAZE_H) {
      Serial.println("CHYBA: mimo mapy!");
      return;
    }

    Node* next = mapa[newX][newY];
    root->addNeigh(curDirection, next);
    int opposite = (curDirection + 2) % 4;
    next->addNeigh(opposite, root);
    root = next;
    curX = newX;
    curY = newY;
  }

  // Cieľový uhol = smer pohybu
  float targetAngle = curDirection * 90.0f;
  unsigned long startTime = millis();
  const unsigned long MOVE_TIME = 800; // ms

  while (millis() - startTime < MOVE_TIME) {
    // Načítaj aktuálny uhol
    sensors_event_t event;
    bno.getEvent(&event);
    float currentAngle = event.orientation.x;

    // Rozdiel uhlov s wrap-around
    float err = targetAngle - currentAngle;
    if (err >  180) err -= 360;
    if (err < -180) err += 360;

    // Korekcia — čím väčšia chyba, tým väčšia korekcia
    // Obmedzená na MAX_CORRECTION aby sa robot nepretočil
    const float KP          = 0.3f; // zosilnenie
    const int   MAX_CORRECT = 15;   // max odchýlka od SPEED

    int correction = (int)(KP * err);
    correction = constrain(correction, -MAX_CORRECT, MAX_CORRECT);

    servoLeft.write(STOP + SPEED + correction);
    servoRight.write(STOP - SPEED + correction);

    delay(10);
  }

  stopMotors();
  delay(100);
}


// ============================================================
// SKENOVANIE — Right-Hand Rule
// ============================================================
void explore() {
  d = readSensors();
  root->visited = true;

  bool wallLeft  = d.left  < 150;
  bool wallFront = d.front < 150;
  bool wallRight = d.right < 150;

  Serial.printf("[%d,%d] L:%s F:%s R:%s Dir:%d\n",
    root->x, root->y,
    wallLeft  ? "X" : "_",
    wallFront ? "X" : "_",
    wallRight ? "X" : "_",
    curDirection);

  if (!wallRight) {
    turn(3);
    goForward();
  } else if (!wallFront) {
    goForward();
  } else if (!wallLeft) {
    turn(1);
    goForward();
  } else {
    turn(1); turn(1);
    goForward();
  }
}

int goToPos(uint8_t nextX, uint8_t nextY) {
  if (curX == nextX && curY == nextY) return 0;

  Node* target = mapa[nextX][nextY];

  // === BFS ===
  std::queue<Node*> bfsQueue;
  std::array<std::array<Node*, 7>, 7> parent;
  std::array<std::array<int8_t, 7>, 7> parentDir; // smer ktorým sme prišli

  for (auto& row : parent)    row.fill(nullptr);
  for (auto& row : parentDir) row.fill(-1);

  parent[root->x][root->y] = root;
  bfsQueue.push(root);

  while (!bfsQueue.empty()) {
    Node* cur = bfsQueue.front();
    bfsQueue.pop();

    if (cur == target) break;

    for (int8_t d = 0; d < 4; d++) {
      Node* nb = cur->neigh[d];
      if (nb && parent[nb->x][nb->y] == nullptr) {
        parent[nb->x][nb->y]    = cur;
        parentDir[nb->x][nb->y] = d;   // smer z cur → nb
        bfsQueue.push(nb);
      }
    }
  }

  if (parent[nextX][nextY] == nullptr) {
    Serial.println("Cesta nenajdena!");
    return -1;
  }

  // === Rekonštrukcia cesty ako zoznam smerov ===
  std::vector<uint8_t> path;
  Node* cur = target;

  while (cur != root) {
    int8_t d = parentDir[cur->x][cur->y];
    path.push_back((uint8_t)d);
    cur = parent[cur->x][cur->y];
  }

  std::reverse(path.begin(), path.end());

  // Debug výpis
  const char* dirSymbol[] = {">", "^", "<", "v"};
  Serial.print("Cesta: ");
  for (uint8_t d : path) Serial.print(dirSymbol[d]);
  Serial.println();

  // === Vykonanie cesty ===
  for (uint8_t d : path) {
    // Otoč sa na požadovaný smer
    int8_t diff = ((int8_t)d - (int8_t)curDirection + 4) % 4;
    if      (diff == 1) turn(3);         // +90° → doprava
    else if (diff == 3) turn(1);         // -90° → doľava
    else if (diff == 2) { turn(1); turn(1); } // 180° → otočenie

    goForward();
  }

  return 0;
}
// ============================================================
// ULOŽENIE DO FLASH
// ============================================================
void saveGraph() {
  if (!LittleFS.begin()) { Serial.println("LittleFS: chyba!"); return; }

  File f = LittleFS.open("/maze.txt", "w");
  if (!f) { Serial.println("Chyba pri otvarani!"); return; }

  for (int y = 0; y < MAZE_H; y++) {
    for (int x = 0; x < MAZE_W; x++) {
      Node* cur = mapa[y][x];
      if (!cur) continue;

      // Format: N: x, y, >, ^, <, v
      f.print("N: ");
      f.print(cur->x); f.print(", ");
      f.print(cur->y); f.print(", ");
      f.print(cur->neigh[0] ? 1 : 0); f.print(", ");
      f.print(cur->neigh[1] ? 1 : 0); f.print(", ");
      f.print(cur->neigh[2] ? 1 : 0); f.print(", ");
      f.println(cur->neigh[3] ? 1 : 0);
    }
  }

  f.close();
  Serial.println("Mapa ulozena do /maze.txt");
}

void setupI2C() {
  BUS.setSDA(I2C_SDA);
  BUS.setSCL(I2C_SCL);
  BUS.begin();
  delay(100);
}
// ============================================================
// SETUP & LOOP
// ============================================================
void setup() {
  setupI2C();
  Serial.begin(115200);
  while (!Serial) delay(10);
  for (int y = 0; y < MAZE_H; y++){
    for (int x = 0; x < MAZE_W; x++){
        mapa[y][x] = new Node(x,y);;
    }
  }
  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);
  stopMotors();

  if (setupGyroscope() != 0) { Serial.println("FATAL: Gyroskop!"); while (1) {} }
  if (setupLasers()    != 0) { Serial.println("FATAL: Lasery!");   while (1) {} }

  root = mapa[0][0];
  Serial.println("System pripraveny. Stlac ENTER pre start...");
  while (!Serial.available()) {}
  Serial.read();
}



void loop() {
  int left  = sensors[0].readRangeContinuousMillimeters();
  int front = sensors[1].readRangeContinuousMillimeters();
  int right = sensors[2].readRangeContinuousMillimeters();
  rotations = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  Serial.print(left);
  Serial.print(" | ");
  Serial.print(front);
  Serial.print(" | ");
  Serial.print(right);
  Serial.print(" | ");
  Serial.print(right);
  Serial.print(" | ");
  Serial.print(rotations.x()); 
  Serial.print(" | ");
  Serial.print(rotations.y());
  Serial.print(" | ");
  Serial.println(rotations.z());

  delay(50);
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
  }
}