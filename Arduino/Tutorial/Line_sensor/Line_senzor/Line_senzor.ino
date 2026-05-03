const uint8_t SENSOR_COUNT = 4;
const uint8_t sensorPins[SENSOR_COUNT] = {26, 27, 28, 29};
const int IR_PIN = 15;
const int THRESHOLD = 400;  // prah medzi bielou a čiernou

void setup() {
  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, HIGH);
  Serial.begin(115200);
}

void loop() {
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    int val = analogRead(sensorPins[i]);
    // Vizualizacia – 1 = ciara, 0 = biela
    Serial.print(val > THRESHOLD ? "1" : "0");
    Serial.print("\t");
  }
  Serial.println();
  delay(100);
}