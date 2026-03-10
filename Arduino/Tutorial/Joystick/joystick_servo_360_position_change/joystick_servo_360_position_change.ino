#include <Servo.h>

Servo servo;

int joyX = A0;

int center = 512;
int deadzone = 40;

int servoValue = 90;

void setup() {
  Serial.begin(115200);
  servo.attach(0); // GP0
}

void loop() {

  int x = analogRead(joyX);
  int dx = x - center;

  if (abs(dx) > deadzone) {
    servoValue = 90 + dx / 10;   // citlivosť
  } else {
    servoValue = 90;             // stop
  }

  servoValue = constrain(servoValue, 0, 180);

  servo.write(servoValue);

  Serial.print("Servo value: ");
  Serial.println(servoValue);

  delay(20);
}