#include <Servo.h>

#define TRIG 3
#define ECHO 2
#define SERVO_PIN 0

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  // Ultrazvuk
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.0343 / 2; // cm

  // Obmedzíme rozsah
  distance = constrain(distance, 5, 100);

  // Prevedieme vzdialenosť na rýchlosť serva
  // bližšie = otáča sa rýchlejšie doprava
  int speed = map(distance, 5, 100, 180, 90); // 90 = stop, 180 = max doprava

  servo.write(speed);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm  Servo speed: ");
  Serial.println(speed);

  delay(50);
}