#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>
#include <Servo.h>
#include <array>
#include <queue>

// === XSHUT piny pre VL53L0X ===
const int xshutPins[3] = {0, 1, 2}; // vľavo, vpredu, vpravo
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
  {-1,  0},  // 2 = <
  { 0, -1}   // 3 = v
};

static uint8_t curDirection = 0;
static uint8_t curX = 0;
static uint8_t curY = 0;

const int   STOP    = 90;
const int   SPEED   = 20;
const float EPSILON = 2.0f;

// === I2C ===
#define I2C_SDA  4
#define I2C_SCL  5
#define BNO_SDA 6
#define BNO_SCL 7

TwoWire &BUS = Wire;

// === Servo piny ===
#define SERVO_LEFT_PIN  9
#define SERVO_RIGHT_PIN 10

VL53L0X sensors[3];
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &BUS);
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
static std::array<std::array<Node*,7>,7> mapa;

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
  } 
  else {
    int newX = root->x + mapDirection[curDirection][0];
    int newY = root->y + mapDirection[curDirection][1];

    if (newX < 0 || newX >= 7 || newY < 0 || newY >= 7) {
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

int goToPos(uint8_t nextX, uint8_t nextY){
 return 1;
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
  for (int y = 0; y < 7; y++){
    for (int x = 0; x < 7; x++){
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

  Serial.print(left);
  Serial.print(" | ");
  Serial.print(front);
  Serial.print(" | ");
  Serial.println(right);

  delay(50);
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
  }
}