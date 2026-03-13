#include <Servo.h>

#define SERVO_PIN 9

int joyX = A0;
int joyY = A1;
int button = 15;

Servo servo;

void setup() {
  Serial.begin(115200);

  pinMode(button, INPUT_PULLUP);

  servo.attach(SERVO_PIN);
}
int speedL = 90;
int speedR = 90;

void loop() {
  int x = analogRead(joyX);
  int y = analogRead(joyY);

  if(y > 1000){ //hore
    speedL = 0;
    speedR = 0;
  }
  else if(y < 16){ //dole
    speedL = 180;
    speedR = 180;
  }
  else if(x > 1000){ //doprava 
    speedL = 0;
    speedR = 180;
  }
  else if(x < 16){ //dolava
    speedL = 1800;
    speedR = 0;
  }
  else{
    speedL = 90;
    speedR = 90;
  }
  
  servo.write(speed);

  Serial.print("Y: ");
  Serial.print(y);
  Serial.print("  Servo: ");
  Serial.println(speed);

  delay(20);
}