#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
#include <Servo.h>
#include <array>

// === XSHUT piny pre VL53L0X ===
const int xshutPins[3] = {6, 7, 8}; // vľavo, vpredu, vpravo
const uint8_t sensorAddrs[3] = {0x30, 0x31, 0x32};

struct SensorData {
  int left, front, right;
  float heading;
  int dir;
};

static SensorData d = {0, 0, 0, 0.0f, 0};


// === Mapa smerov: 0=>  1=^  2=<  3=v ===
const int8_t mapDirection[4][2] = {
  { 1,  0},  // 0 = >
  { 0,  1},  // 1 = ^
  {-1,  0},  // 2 = 
  { 0, -1}   // 3 = v
};

uint8_t curDirection = 0;
const int   STOP    = 90;
const int   SPEED   = 20;
const float EPSILON = 2.0f;

// === I2C ===
#define VL_SDA  0
#define VL_SCL  1
#define BNO_SDA 2
#define BNO_SCL 3

// === Servo piny ===
#define SERVO_LEFT_PIN  9
#define SERVO_RIGHT_PIN 10

VL53L0X sensors[3];
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire1);
Servo servoLeft, servoRight;
bool gyroActive = false;

// ============================================================
// NODE — graf bludiska
// ============================================================
class Node {
  public:
    std::array<Node*, 4> neigh; // 0=> 1=^ 2=< 3=v
    int x, y;
    bool visited;

    Node(int _x, int _y) : x(_x), y(_y), visited(false) {
      neigh.fill(nullptr);
    }

    void addNeigh(int dir, Node* node) {
      if (dir >= 0 && dir <= 3) neigh[dir] = node;
    }

    Node* getNeigh(int dir) {
      if (dir >= 0 && dir <= 3) return neigh[dir];
      return nullptr;
    }
};

Node* root = nullptr;

// ============================================================
// SETUP GYROSKOP
// ============================================================
int setupGyroscope() {
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

  if (foundAddr < 0) { Serial.println("BNO055: nenajdene!"); return -1; }

  bno = Adafruit_BNO055(55, foundAddr, &Wire1);
  if (!bno.begin()) { Serial.println("BNO055 begin() zlyhalo!"); return -2; }

  gyroActive = true;
  Serial.println("BNO055 OK");
  return 0;
}

// ============================================================
// SETUP VL53L0X
// ============================================================
int setupLasers() {
  Wire.setSDA(VL_SDA);
  Wire.setSCL(VL_SCL);
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
  // Aktualizuj graf
  if (root->neigh[curDirection] != nullptr) {
    root = root->neigh[curDirection];
  } else {
    int newX = root->x + mapDirection[curDirection][0];
    int newY = root->y + mapDirection[curDirection][1];
    Node* next = new Node(newX, newY);
    root->addNeigh(curDirection, next);
    int opposite = (curDirection + 2) % 4;
    next->addNeigh(opposite, root);
    root = next;
  }

  // Cieľový uhol = smer pohybu
  float targetAngle = curDirection * 90.0f;
  unsigned long startTime = millis();
  const unsigned long MOVE_TIME = 800; // ms — kalibruj podľa bunky

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
    const float KP          = 0.3f; // zosilnenie — kalibruj
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

// ============================================================
// ULOŽENIE MAPY — Serial výstup pre PC
// ============================================================
void sendMap() {
  // Nájdi rozsah x,y v grafe (BFS)
  int minX = 0, maxX = 0, minY = 0, maxY = 0;

  // BFS na nájdenie rozmerov
  std::array<Node*, 256> queue;
  std::array<Node*, 256> visited;
  int head = 0, tail = 0, visitedCount = 0;
  queue[tail++] = root;

  while (head != tail) {
    Node* cur = queue[head++];
    bool alreadyVisited = false;
    for (int i = 0; i < visitedCount; i++) {
      if (visited[i] == cur) { alreadyVisited = true; break; }
    }
    if (alreadyVisited) continue;
    visited[visitedCount++] = cur;

    if (cur->x < minX) minX = cur->x;
    if (cur->x > maxX) maxX = cur->x;
    if (cur->y < minY) minY = cur->y;
    if (cur->y > maxY) maxY = cur->y;

    for (int i = 0; i < 4; i++) {
      if (cur->neigh[i] != nullptr) queue[tail++] = cur->neigh[i];
    }
  }

  int width  = maxX - minX + 1;
  int height = maxY - minY + 1;

  // Vytvor 2D mapu
  char maze[16][16];
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      maze[y][x] = '?';

  // Znovu BFS — tentokrát kresli
  head = tail = visitedCount = 0;
  queue[tail++] = root;

  while (head != tail) {
    Node* cur = queue[head++];
    bool alreadyVisited = false;
    for (int i = 0; i < visitedCount; i++) {
      if (visited[i] == cur) { alreadyVisited = true; break; }
    }
    if (alreadyVisited) continue;
    visited[visitedCount++] = cur;

    int mx = cur->x - minX;
    int my = cur->y - minY;
    maze[my][mx] = cur->visited ? ' ' : '?';

    // Steny — bunky bez suseda
    for (int i = 0; i < 4; i++) {
      if (cur->neigh[i] != nullptr) {
        queue[tail++] = cur->neigh[i];
      }
    }
  }

  // Štartovná pozícia
  maze[-minY][-minX] = 'S';

  // Odošli na PC
  Serial.println("MAZE_START");
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) Serial.print(maze[y][x]);
    Serial.println();
  }
  Serial.println("MAZE_END");
}

// ============================================================
// SETUP & LOOP
// ============================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);
  stopMotors();

  if (setupGyroscope() != 0) { Serial.println("FATAL: Gyroskop!"); while (1) {} }
  if (setupLasers()    != 0) { Serial.println("FATAL: Lasery!");   while (1) {} }

  root = new Node(0, 0);
  Serial.println("System pripraveny. Stlac ENTER pre start...");
  while (!Serial.available()) {}
  Serial.read();
}

void loop() {
  explore();

  // Pošli mapu po každých 10 krokoch alebo na príkaz "GET"
  static int steps = 0;
  if (++steps % 10 == 0) sendMap();

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "GET") sendMap();
  }
}