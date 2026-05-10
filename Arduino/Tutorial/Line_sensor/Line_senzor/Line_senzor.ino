/*
  RP2040                74HC4051             QTR-8A 
┌──────────┐ ┌──────────────────────────┐  ┌────────┐
|      5V  ├─┘        ┌──────────┐      └──┤ IR     |
|      3.3V├──────────┤VCC    VCC├─────────┤ VCC    |
│      GP26├──────────┤Z      GND├─────────┤ GND    | 
│      GP13├──────────┤S2      Y0├─────────┤ D1     |
│      GP14├──────────┤S1      Y1├─────────┤ D2     |
│      GP15├──────────┤S0      Y2├─────────┤ D3     |
│       GND├──────────┤GND     Y3├─────────┤ D4     |
└──────────┘          |        Y4├─────────┤ D5     |
                      |        Y5├─────────┤ D6     |
                      |        Y6├─────────┤ D7     |
                      |        Y7├─────────┤ D8     |
                      └──────────┘         └────────┘
*/
const int S_A    = 15;
const int S_B    = 14;
const int S_C    = 13;
const int MUX_OUT = 26;
const int NUM_SENSORS = 8;
const int threshold = 3000
int sensorValues[NUM_SENSORS];

void selectChannel(int ch) {
  digitalWrite(S_A, (ch >> 0) & 1);
  digitalWrite(S_B, (ch >> 1) & 1);
  digitalWrite(S_C, (ch >> 2) & 1);
  delayMicroseconds(2);
}
void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    selectChannel(i);
    sensorValues[i] = analogRead(MUX_OUT);
  }
}

String getLinePosition() {
  String state = "";
  for (int i = 0; i < NUM_SENSORS; i++) {
    int val = sensorValues[i];
    String tmp = ".";
    if (val > threshold) tmp = "#";
    state += tmp;
  }
  return state;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  pinMode(S_A, OUTPUT);
  pinMode(S_B, OUTPUT);
  pinMode(S_C, OUTPUT);
  analogReadResolution(12);
  Serial.println("=== Line senzor test ===");
  Serial.println("S1   S2   S3   S4   S5   S6   S7   S8   | Pozicia");
  Serial.println("------------------------------------------------");
}
void loop() {
  readSensors();
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.printf("%4d ", sensorValues[i]);
  }
  String pos = getLinePosition();
  Serial.printf(" | ");
  Serial.println(pos);

  delay(100);
}