#include <Servo.h>

// RP2040-Zero pinout
const int PIN_SERVO_LEFT_WHEEL = 4;
const int PIN_SERVO_RIGHT_WHEEL = 5;
const int PIN_SERVO_SCANER = 6;
const int TRIG = 7;
const int ECHO = 8;

Servo servoLeftWheel;
Servo servoRightWheel;
Servo servoScaner;

const int SPEED = 90;
const float MIN_SAFE_DISTANCE = 20.0;
const long PULSIN_TIMEOUT = 30000;

bool blocking = false;

// ================== ULTRASONIC ==================
float scanDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, PULSIN_TIMEOUT);

  if (duration == 0) {
    Serial.println("Senzor timeout!");
    return -1;
  }

  float distance = duration * 0.0343 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

// ================== POHYB ==================
void stopRobot() {
  servoLeftWheel.write(90);
  servoRightWheel.write(90);
}

void moveForward() {
  servoLeftWheel.write(90 + SPEED);
  servoRightWheel.write(90 - SPEED);
}

void moveBackward() {
  servoLeftWheel.write(90 - SPEED);
  servoRightWheel.write(90 + SPEED);
}

// ================== ROTÁCIA ==================
void rotateLeft(int angle) {
  servoLeftWheel.write(110);
  servoRightWheel.write(110);

  int durationMs = angle * 14; // doladiť
  delay(durationMs);

  stopRobot();
}

void rotateRight(int angle) {
  servoLeftWheel.write(70);
  servoRightWheel.write(70);

  int durationMs = angle * 14; // doladiť
  delay(durationMs);

  stopRobot();
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servoLeftWheel.attach(PIN_SERVO_LEFT_WHEEL, 1000, 2000);
  servoRightWheel.attach(PIN_SERVO_RIGHT_WHEEL, 1000, 2000);
  servoScaner.attach(PIN_SERVO_SCANER, 500, 2500);

  stopRobot();
  servoScaner.write(90);

  Serial.println("READY");
}

// ================== LOOP ==================
void loop() {

  // dopredu
  moveForward();
  delay(50);

  float distFront = scanDistance();
  blocking = distFront > 0 && distFront <= MIN_SAFE_DISTANCE;

  while (blocking) {
    Serial.println("⚠️ Prekážka!");

    stopRobot();
    delay(100);

    // ===== POZRI VĽAVO =====
    servoScaner.write(0);
    delay(300);
    float distLeft = scanDistance();

    if (distLeft > MIN_SAFE_DISTANCE || distLeft < 0) {
      Serial.println("✓ Vľavo voľné");
      servoScaner.write(90);
      rotateLeft(90);
      return;
    }

    // ===== POZRI VPRAVO =====
    servoScaner.write(180);
    delay(600);
    float distRight = scanDistance();

    if (distRight > MIN_SAFE_DISTANCE || distRight < 0) {
      Serial.println("✓ Vpravo voľné");
      servoScaner.write(90);
      rotateRight(90);
      return;
    }

    // ===== NIČ VOĽNÉ =====
    Serial.println("✗ Nikde voľné -> cúvam");

    servoScaner.write(90);
    moveBackward();
    delay(800);
    stopRobot();
    delay(200);
  }

  Serial.println("✓ Voľno dopredu");
  delay(200);
}