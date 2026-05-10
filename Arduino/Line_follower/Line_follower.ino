#include <Servo.h>

// ============= PINY =============
const int S_A     = 15;
const int S_B     = 14;
const int S_C     = 13;
const int MUX_OUT = 26;

const int NUM_SENSORS = 8;
const int THRESHOLD   = 3600;

int sensorValues[NUM_SENSORS];

// ============= SERVO =============
Servo leftServo;
Servo rightServo;
const int LeftServoPin  = 10;
const int RightServoPin = 11;

const int STOP      = 90;
const int BASE_SPEED = 60;  // offset od 90, uprav podľa rýchlosti

// ============= PID =============
float Kp = 0.08;   // proporcionálna zložka  ← hlavne toto ladi
float Ki = 0.0001; // integrálna zložka      ← nechaj malé
float Kd = 0.8;    // derivačná zložka       ← tlmí kmitanie

float lastError   = 0;
float integral    = 0;

// Stred = 3500 (senzory 0-7, váhované * 1000)
// 0 = úplne vľavo, 7000 = úplne vpravo
float getLinePosition() {
  long weightedSum = 0;
  long sum = 0;

  for (int i = 0; i < NUM_SENSORS; i++) {
    int flipped = (NUM_SENSORS - 1 - i);
    if (sensorValues[i] > THRESHOLD) {
      weightedSum += (long)sensorValues[i] * i * 1000;
      sum += sensorValues[i];
    }
  }
  if (sum == 0) return -1; // čiara nenájdená
  return (float)weightedSum / sum;
}

uint8_t readLine() {
  uint8_t val = 0;
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    if (sensorValues[i] > THRESHOLD) {
      val |= (1 << (i));
    }
  }
  return val;
}

uint8_t countLine(uint8_t state) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    if (state & (1 << i)) count++;
  }
  return count;
}

// ============= MUX =============
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

// ============= POHYB =============
void setMotors(int correction) {
  // correction > 0 = čiara vpravo → zatočiť vpravo
  // correction < 0 = čiara vľavo  → zatočiť vľavo
  int leftSpeed  = STOP + BASE_SPEED + correction;
  int rightSpeed = STOP - BASE_SPEED + correction;

  // Obmedzenie na rozsah servo 0-180
  leftSpeed  = constrain(leftSpeed,  0, 180);
  rightSpeed = constrain(rightSpeed, 0, 180);

  rightServo.write(leftSpeed);
  leftServo.write(rightSpeed);
}

void emergencyStop() {
  leftServo.write(STOP);
  rightServo.write(STOP);
  integral  = 0;
  lastError = 0;
}

// ============= AVOID BRICK =============
void avoidBrick() {
  emergencyStop();
  delay(300);
  // vpravo
  leftServo.write(STOP + 30);
  rightServo.write(STOP + 30);
  delay(500);
  // dopredu
  setMotors(0);
  delay(1500);
  // vľavo
  leftServo.write(STOP - 30);
  rightServo.write(STOP - 30);
  delay(500);
  setMotors(0);
  delay(2000);
  leftServo.write(STOP - 30);
  rightServo.write(STOP - 30);
  delay(500);
  setMotors(0);
  delay(1500);
  // späť na čiaru
  leftServo.write(STOP + 30);
  rightServo.write(STOP + 30);
  delay(500);
}

// ============= SETUP =============
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(S_A, OUTPUT);
  pinMode(S_B, OUTPUT);
  pinMode(S_C, OUTPUT);
  analogReadResolution(12);

  leftServo.attach(LeftServoPin);
  rightServo.attach(RightServoPin);
  emergencyStop();
  delay(1000);

  Serial.println(">>> SYSTEM READY <<<");
}

// ============= HLAVNÝ LOOP =============

void loop() {
  readSensors();
  uint8_t state = readLine();
  uint8_t count = countLine(state);
  for (int i = 1; i <= NUM_SENSORS; i++){
    Serial.print(sensorValues[NUM_SENSORS-i]);
    Serial.print(" ");
  }
  // Križovatka / koniec = 5+ senzorov naraz
  if (count == 8) {
    emergencyStop();
    Serial.println("Krizovatka / koniec");
    delay(200);
    return;
  }

  // Čiara nenájdená
  if (count == 0) {
    emergencyStop();
    leftServo.write(90 + BASE_SPEED);
    rightServo.write(90 - BASE_SPEED);
    
    Serial.println("Ciara nenajdena");
    delay(200);
    return;
  }

  // ============= PID =============
  float position = getLinePosition();
  float error    = position - 3500.0;

  integral  += error;
  integral   = constrain(integral, -5000, 5000);

  float derivative = error - lastError;
  lastError = error;

  float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);
  correction = constrain(correction, -BASE_SPEED, BASE_SPEED);

  setMotors((int)correction);

  // DEBUG
  Serial.print("cnt:");
  Serial.print(count);
  Serial.print(" pos:");
  Serial.print(position);
  Serial.print(" err:");
  Serial.print(error);
  Serial.print(" cor:");
  Serial.println(correction);

  delay(20);
}