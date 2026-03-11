#include <Servo.h>

int joyX = A0;
int joyY = A1;
int button = 15;

Servo servo;

void setup() {
  Serial.begin(115200);

  pinMode(button, INPUT_PULLUP);

  servo.attach(9);   // pin serva
}

void loop() {

  int x = analogRead(joyX);

  // mapovanie joysticku na servo
  int speed = map(x, 0, 4095, 0, 180)+57;

  servo.write(speed);

  Serial.print("X: ");
  Serial.print(x);
  Serial.print("  Servo: ");
  Serial.println(speed);

  delay(20);
}