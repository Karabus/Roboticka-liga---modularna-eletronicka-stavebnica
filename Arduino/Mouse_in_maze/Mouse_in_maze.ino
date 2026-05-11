#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <VL53L0X.h>

// === XSHUT piny pre VL53L0X ===
const int xshutPins[3] = {6, 7, 8}; // vľavo, vpredu, vpravo
const uint8_t sensorAddrs[3] = {0x30, 0x31, 0x32};

// === I2C pre VL53L0X (Wire) a BNO055 (Wire1) ===
#define VL_SDA  0
#define VL_SCL  1
#define BNO_SDA 2
#define BNO_SCL 3

VL53L0X sensors[3];
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire1);
bool gyroActive = false;

// ============================================================
// NODE — graf bludiska
// ============================================================
class Node {
  public:
    Node* neigh[4]; // 0=vľavo, 1=hore, 2=vpravo, 3=dole
    int x, y;

    Node(int _x, int _y) {
      x = _x;
      y = _y;
      for (int i = 0; i < 4; i++) neigh[i] = NULL;
    }

    void addNeigh(int i, Node* node) {
      if (i >= 0 && i <= 3) neigh[i] = node;
    }
};

Node* root = NULL;

// ============================================================
// SETUP GYROSKOP
// ============================================================
int setupGyroscope() {
  Wire1.setSDA(BNO_SDA);
  Wire1.setSCL(BNO_SCL);
  Wire1.begin();
  delay(100);

  Serial.println("Hladam BNO055...");

  // I2C scan
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

  if (foundAddr < 0) {
    Serial.println("BNO055: nenajdene!");
    return -1;
  }

  bno = Adafruit_BNO055(55, foundAddr, &Wire1);
  if (!bno.begin()) {
    Serial.println("BNO055 begin() zlyhalo!");
    return -2;
  }

  gyroActive = true;
  Serial.println("BNO055 OK");
  return 0;
}

// ============================================================
// SETUP VL53L0X (cez XSHUT + jeden I2C bus)
// ============================================================
int setupLasers() {
  Wire.setSDA(VL_SDA);
  Wire.setSCL(VL_SCL);
  Wire.begin();

  // 1. Vypni všetky senzory
  for (int i = 0; i < 3; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(10);

  // 2. Jeden po druhom zapni a premenuj
  for (int i = 0; i < 3; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(10);

    sensors[i].setBus(&Wire);

    if (!sensors[i].init()) {
      Serial.print("VL53L0X #");
      Serial.print(i);
      Serial.println(" nenajdeny!");
      return -1;
    }

    sensors[i].setAddress(sensorAddrs[i]);

    Serial.print("VL53L0X #");
    Serial.print(i);
    Serial.print(" OK → 0x");
    Serial.println(sensorAddrs[i], HEX);
  }

  // 3. Nastav kontinuálny mód
  for (int i = 0; i < 3; i++) {
    sensors[i].startContinuous(50); // meranie každých 50ms
  }

  Serial.println("Vsetky senzory OK");
  return 0;
}

// ============================================================
// ČÍTANIE SENZOROV
// ============================================================
struct SensorData {
  int left;   // mm
  int front;  // mm
  int right;  // mm
  float heading; // stupne 0-360
};

SensorData readSensors() {
  SensorData data;
  data.left  = sensors[0].readRangeContinuousMillimeters();
  data.front = sensors[1].readRangeContinuousMillimeters();
  data.right = sensors[2].readRangeContinuousMillimeters();

  if (gyroActive) {
    sensors_event_t event;
    bno.getEvent(&event);
    data.heading = event.orientation.x;
  } else {
    data.heading = -1;
  }

  return data;
}

// ============================================================
// SETUP & LOOP
// ============================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (setupGyroscope() != 0) {
    Serial.println("FATAL: Gyroskop zlyhal!");
    while (1) {}
  }

  if (setupLasers() != 0) {
    Serial.println("FATAL: Lasery zlyhali!");
    while (1) {}
  }

  // Vytvor koreňový uzol bludiska
  root = new Node(0, 0);
  Serial.println("System pripraveny.");
}

void loop() {
  SensorData d = readSensors();

  Serial.printf("L:%4dmm  F:%4dmm  R:%4dmm  Heading:%.1f°\n",
                d.left, d.front, d.right, d.heading);
  delay(100);
}