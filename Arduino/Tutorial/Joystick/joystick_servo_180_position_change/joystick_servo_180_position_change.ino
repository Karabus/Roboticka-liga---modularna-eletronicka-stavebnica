#include <Servo.h>

Servo servoX;
Servo servoY;

int joyX = A0;
int joyY = A1;

float servoXPos = 90;
float servoYPos = 90;

int center = 512;
int deadzone = 40;

void setup() {
  Serial.begin(115200);

  servoX.attach(0); // GP0
  servoY.attach(1); // GP1

  servoX.write(servoXPos);
  servoY.write(servoYPos);
}

void loop() {

  int x = analogRead(joyX);
  int y = analogRead(joyY);

  int dx = x - center;
  int dy = y - center;

  if (abs(dx) > deadzone) {
    servoXPos += dx / 200.0;   // citlivosť
  }

  if (abs(dy) > deadzone) {
    servoYPos += dy / 200.0;
  }

  servoXPos = constrain(servoXPos, 0, 180);
  servoYPos = constrain(servoYPos, 0, 180);

  servoX.write(servoXPos);
  servoY.write(servoYPos);

  Serial.print("Xpos: ");
  Serial.print(servoXPos);
  Serial.print("  Ypos: ");
  Serial.println(servoYPos);

  delay(20);
}