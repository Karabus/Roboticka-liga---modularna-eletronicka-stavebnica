#include <Servo.h>

Servo servoX;
Servo servoY;

int joyX = A0;
int joyY = A1;

void setup() {
  Serial.begin(115200);

  servoX.attach(0);   // GP0
  servoY.attach(1);   // GP1
}

void loop() {

  int x = analogRead(joyX);
  int y = analogRead(joyY);

  int angleX = map(x, 0, 1023, 0, 180);
  int angleY = map(y, 0, 1023, 0, 180);

  servoX.write(angleX);
  servoY.write(angleY);

  Serial.print("ServoX: ");
  Serial.print(angleX);
  Serial.print("  ServoY: ");
  Serial.println(angleY);

  delay(20);
}