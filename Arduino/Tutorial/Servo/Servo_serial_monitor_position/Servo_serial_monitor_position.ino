#include <Servo.h>

#define SERVO_PIN 12

String inputString = "";
Servo servo;

int position = 180;
void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN,500,2500);
  servo.write(position);
  delay(1000);
  servo.write(2);  
}

void loop() {
/*
   while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // Enter
      position = inputString.toInt();
      if (position >= 500 && position <= 2500) {
        servo.write(position); // nastavi servo na uhol
        Serial.print(" Servo je nastavny na uhli: ");
        Serial.println(servo.read());
      } else {
        Serial.println("Neplatna pozicia");
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }

  delay(50);*/
}