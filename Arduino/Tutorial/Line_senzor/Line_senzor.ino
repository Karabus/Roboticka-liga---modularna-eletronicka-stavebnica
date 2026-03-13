// ============================================
// 8-CH Line Follower + 74HC4051 Multiplexer
// ============================================

#define S0  4    // MUX select bit 0
#define S1  5    // MUX select bit 1
#define S2  6    // MUX select bit 2
#define SIG A0  // MUX výstup → Arduino analog
#define THRESHOLD   500
#define NUM_SENSORS  8

int  sensorValues[NUM_SENSORS];
bool onLine[NUM_SENSORS];

void setup() {
  Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
}

void selectChannel(uint8_t ch) {
  digitalWrite(S0, (ch >> 0) & 1);
  digitalWrite(S1, (ch >> 1) & 1);
  digitalWrite(S2, (ch >> 2) & 1);
  delayMicroseconds(10);
}

// Prečítaj všetkých 8 senzorov
void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    selectChannel(i);
    sensorValues[i] = analogRead(SIG);
    onLine[i]        = (sensorValues[i] < THRESHOLD);
  }
}

// Pozícia čiary: -350 (vľavo) → 0 (stred) → +350 (vpravo)
float getPosition() {
  long ws = 0; int tot = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (onLine[i]) { ws += (i - 3.5f) * 100; tot++; }
  }
  if (tot == 0) return 999;   // čiara stratená!
  return (float)ws / tot;
}

void loop() {
  readSensors();

  Serial.print("Senzory: ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(onLine[i] ? "■ " : "□ ");
  }
  float pos = getPosition();
  Serial.print("  Pozícia: ");
  Serial.println(pos == 999 ? "STRATENÁ" : String(pos));

  delay(20);
}