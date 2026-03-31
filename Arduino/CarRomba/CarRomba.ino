#include <Servo.h>

// ── Servo objekty ─────────────────────────
Servo servoLeftWheel;
Servo servoRightWheel;
Servo servoScaner;

// ── Piny ─────────────────────────────────
const int PIN_SERVO_LEFT_WHEEL = 0;
const int PIN_SERVO_RIGHT_WHEEL = 1;
const int PIN_SERVO_SCANER = 2;
const int TRIG = 3;
const int ECHO = 4;

// ── Nastavenia ───────────────────────────
const int SERVO_LEFT    = 0;
const int SERVO_RIGHT    = 180;
const int SERVO_CENTER = 90;

// ── Stav ─────────────────────────────────
float posArm1 = 90.0f;
float posArm2 = 90.0f;
float posGrapper = 0.0f;
int rotationDir = 90; //0 - left, 90 - stop, 180 - right 

float scanDistance(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  
  float distance = duration * 0.0343 / 2;
  return distance;
}

void turnRight(int angle){
  servoRightWheel.write(180);
  servoLeftWheel.write(180);
  delay(20*angle);
  return;
}

void turnLeft(int angle){
  servoRightWheel.write(0);
  servoLeftWheel.write(0);
  delay(20*angle);
  return;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servoLeftWheel.attach(PIN_SERVO_LEFT_WHEEL);
  servoRightWheel.attach(PIN_SERVO_RIGHT_WHEEL);
  servoScaner.attach(PIN_SERVO_SCANER);
  
  servoRightWheel.write(0);
  servoLeftWheel.write(180);
  servoScaner.write(90);
  
}

void loop() {
  float distance =  scanDistance();
  //
  if (distance < 10) {
    servoRightWheel.write(90);
    servoLeftWheel.write(90);
    servoScaner.write(135);
    delay(100);
    distance = scanDistance();
    if (distance >= 10) {
      turnRight(45);
    }
    else{
      servoScaner.write(45);  
      distance = scanDistance();
      if (distance >= 10) {
        turnLeft(45);
      }
      else {
        servoRightWheel.write(180);
        servoLeftWheel.write(0);   
      }
    }  
    servoScaner.write(90);  
  }
  else{
    servoRightWheel.write(0);
    servoLeftWheel.write(180);    
  }

  //todo
}
