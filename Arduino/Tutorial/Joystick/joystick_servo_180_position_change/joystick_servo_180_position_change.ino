#include <Servo.h>

Servo servoX;
Servo servoY;

int joyX = A0;
int joyY = A1;

float servoXPos = 90;
float servoYPos = 90;

int center = 740;
int deadzone = 200;

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


  if (x > center + deadzone) {
    servoXPos += 1;   
  }
  else if (x < center - deadzone){
    servoXPos-=1;
  }

  if (y > center + deadzone) {
    servoYPos += 1;
  }
  else if (y < center - deadzone){
    servoYPos-=1;
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