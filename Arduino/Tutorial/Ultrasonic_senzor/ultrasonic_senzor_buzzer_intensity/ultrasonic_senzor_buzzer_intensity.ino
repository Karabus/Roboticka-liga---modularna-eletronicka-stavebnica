#include <Arduino.h>

#define TRIG 3
#define ECHO 2
#define BUZZER 0

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // spustenie merania
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.0343 / 2; // vzdialenosť v cm
  distance = constrain(distance, 5, 100); // limit rozsahu

  // mapovanie vzdialenosti na frekvenciu (Hz)
  int freq = map(distance, 5, 100, 2000, 200); // bližšie = vyššia frekvencia

  tone(BUZZER, freq);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm  Frequency: ");
  Serial.println(freq);

  delay(50);
}