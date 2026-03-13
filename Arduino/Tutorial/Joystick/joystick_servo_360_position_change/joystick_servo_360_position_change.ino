#include <Servo.h>

int joyY = A1;
int button = 15;

Servo servo;

void setup() {
  Serial.begin(115200);

  pinMode(button, INPUT_PULLUP);

  servo.attach(9);   // pin serva
}

void loop() {

  int y = analogRead(joyY);

  // mapovanie joysticku na servo
  int speed = map(y, 0, 4095, 0, 180)+57;

  servo.write(speed);

  Serial.print("Y: ");
  Serial.print(y);
  Serial.print("  Servo: ");
  Serial.println(speed);

  delay(20);
}