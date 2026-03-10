#include <Servo.h>

#define TRIG 3
#define ECHO 2

Servo servo;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servo.attach(0); // GP0
}

void loop() {

  // spustenie merania
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);

  float distance = duration * 0.0343 / 2;

  // obmedzíme rozsah
  distance = constrain(distance, 5, 100);

  // mapovanie vzdialenosti na uhol
  int angle = map(distance, 5, 100, 180, 0);

  servo.write(angle);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm  Angle: ");
  Serial.println(angle);

  delay(100);
}