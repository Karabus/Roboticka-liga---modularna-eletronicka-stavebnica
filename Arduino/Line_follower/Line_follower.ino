#include <Servo.h>

const uint8_t SENSOR_COUNT = 4;
const uint8_t sensorPins[SENSOR_COUNT] = {26, 27, 28, 29};
const int IR_PIN = 15;
const int THRESHOLD = 400;

// Servo motory
Servo leftServo;
Servo rightServo;
const int LeftServoPin = 10;
const int RightServoPin = 11;

// PID premenné
int D = 0, P = 0, I = 0, previousError = 0, error = 0, PIDvalue = 0;
int lsp, rsp;

// ============= INVERZIA MOTORA =============
// Pravý motor je opačne orientovaný
int invertServo(int value) {
  return 180 - value;
}

void avoidBrick(){
  rightServo.write(90);
  leftServo.write(150);
  //otoci sa do prava
  rightServo.write(150);
  delay(500);
  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(1500);
  //ide do lava
  rightServo.write(30);
  leftServo.write(30);
  delay(500);
  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(2000);

  //otoci sa do lava
  rightServo.write(30);
  leftServo.write(30);
  delay(500);
  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(1500);
  //otoci sa doprava
  leftServo.write(150);
  rightServo.write(150);
  delay(500);
  return;
}
uint8_t readLine() {
  uint8_t val = 0;
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    val = val + ((analogRead(sensorPins[i])  > THRESHOLD ? 1 : 0) << i);
  }
  return val;
}


// ============= PID REGULÁCIA =============
void linefollow(int error) {
  return;
}

// ============= EMERGENCY STOP =============
void emergencyStop() {
  Serial.println("🛑 EMERGENCY STOP!");
  leftServo.write(90);
  rightServo.write(90);
}

// ============= SETUP =============
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  leftServo.attach(LeftServoPin);
  rightServo.attach(RightServoPin);

  delay(1000);
  Serial.println("\n>>> SYSTÉM PRIPRAVENÝ - ŠTARTOVANIE <<<\n");
}

// ============= HLAVNÝ LOOP =============
void loop() {
  uint8_t  reading = readLine();
  
  // DEBUG výstup
  Serial.print("SENZORY:[");
  Serial.print(reading);
  Serial.println("]");
  delay(500);

}