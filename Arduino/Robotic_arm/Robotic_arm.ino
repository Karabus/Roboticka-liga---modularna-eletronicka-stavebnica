#include <Servo.h>

// --- Joysticky ---
const int JOY1X = A0;
const int JOY1Y = A1;
const int JOY2X = A2;
const int JOY2Y = A3;

// --- Servá ---
const int PIN_SERVO_360 = 4;
const int PIN_SERVO_UP = 5;
const int PIN_SERVO_FORWARD = 6;
const int PIN_SERVO_GRIPPER = 7;

// --- Tlačidlo ---
const int JOY1_BUTTON = 15;

Servo servo360;
Servo servoUp;
Servo servoForward;
Servo servoGripper;

// --- Stredy joystickov (TVOJE) ---
const int CENTER = 550;

// --- Nastavenia ---
const int DEADZONE = 60;
const int STEP = 1;
const int SERVO_STOP = 90;

// --- Stav ---
bool gripperOpen = false;
int upPos = 45;
int forwardPos = 170;

// --- pomocná ---
int deadzone(int v) {
  if (abs(v) < DEADZONE) return 0;
  return v;
}

void setup() {
  Serial.begin(115200);

  pinMode(JOY1_BUTTON, INPUT_PULLUP);

  servo360.attach(PIN_SERVO_360, 500, 2500);
  servoUp.attach(PIN_SERVO_UP, 500, 2500);
  servoForward.attach(PIN_SERVO_FORWARD, 500, 2500);
  servoGripper.attach(PIN_SERVO_GRIPPER, 500, 2500);

  servo360.write(SERVO_STOP);
  servoUp.write(upPos);
  servoForward.write(forwardPos);
  servoGripper.write(90);
}

void loop() {
  // ========= ČÍTANIE =========
  int y1 = analogRead(JOY1Y);
  int x1 = analogRead(JOY1X);
  int x2 = analogRead(JOY2X);

  // ========= ROTÁCIA =========
  if (y1 < 40) {
    servo360.write(SERVO_STOP - 25);
  } else if (y1 > 1000) {
    servo360.write(SERVO_STOP + 25);
  } else {
    servo360.write(SERVO_STOP);
  }

  // ========= HORE / DOLE =========
  if (x1 < 40) {
    upPos -= STEP;
  } else if (x1 > 1000) {
    upPos += STEP;
  }

  // ========= DOPREDU / DOZADU =========
  if (x2 < 40) {
    forwardPos -= STEP;
  } else if (x2 > 1000) {
    forwardPos += STEP;
  }

  // ========= LIMITY =========
  upPos = constrain(upPos, 0, 180);
  forwardPos = constrain(forwardPos, 90, 180);

  // ========= ZÁPIS =========
  servoUp.write(upPos);
  servoForward.write(forwardPos);

  // ========= TLAČIDLO =========
  static bool last = HIGH;
  bool now = digitalRead(JOY1_BUTTON);

  if (last == HIGH && now == LOW) {
    gripperOpen = !gripperOpen;

    if (gripperOpen) servoGripper.write(90);
    else             servoGripper.write(180);

    delay(200);
  }

  last = now;

  delay(10);
}