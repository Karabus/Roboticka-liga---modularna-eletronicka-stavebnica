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

const float MIN_SAFE_DISTANCE = 50.0; // cm
const long PULSIN_TIMEOUT = 30000;
static bool located = LOW;
static float CurDistance = 0.0f;
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

void stopRobot() {
  servoLeftWheel.write(90);
  servoRightWheel.write(90);
  Serial.println("=== Stop");
}


void moveForward(int speed) {
  servoLeftWheel.write(90+speed);   
  servoRightWheel.write(90-speed);    
  Serial.println(">>> Dopredu");
}

void moveBackward(int speed) {
  servoLeftWheel.write(90-speed);     
  servoRightWheel.write(90+speed);  
  Serial.println("<<< Dozadu");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  delay(1000);
  Serial.println("\n=== SETUP START ===");
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  servoLeftWheel.attach(PIN_SERVO_LEFT_WHEEL, 500, 2500);
  servoRightWheel.attach(PIN_SERVO_RIGHT_WHEEL, 500, 2500);
  servoScaner.attach(PIN_SERVO_SCANER, 500, 2500);
  
  delay(500);
  servoScaner.write(90);

  
  if (servoScaner.read() == 90) {
    Serial.println("=== SETUP DONE ===\n");
  }
  else {
    Serial.println("=== ERROR IN SCANER ===\n");
    while(true){};
  }
  servoLeftWheel.write(90);
  servoRightWheel.write(90);
  
  /*float curMin = 50000.0f;
  int curMinIndex = -1;
  
  for (int i = 0; i < 18; i++){
    servoScaner.write(i*10+5);
    delay(200);
    CurDistance = scanDistance();
    if (CurDistance != -1.0f && curMin < CurDistance){
      curMin = CurDistance;
      curMinIndex = i;
    }
  }
  servoScaner.write(curMinIndex*10+5);*/
  
}

void loop() {
  delay(20);

  CurDistance = scanDistance();

  // ignoruj chyby senzora
  if (CurDistance == -1) return;

  // hysteréza: mŕtva zóna medzi 20 a 30 cm
  if (CurDistance < 20) {
    // príliš blízko → cúvni
    int speed = map(CurDistance, 0, 20, 90, 20); // 20 = minimálny pohyb, 90 = max
    speed = constrain(speed, 20, 90);
    moveBackward(speed);
  } 
  else if (CurDistance > 20 && CurDistance < 50) {
    // príliš ďaleko → priblíž sa
    int speed = map(CurDistance, 30, 50, 20, 90);
    speed = constrain(speed, 20, 90);
    moveForward(speed);
  } 
  else {
    // v bezpečnej vzdialenosti → stoj
    stopRobot();
  }
}

