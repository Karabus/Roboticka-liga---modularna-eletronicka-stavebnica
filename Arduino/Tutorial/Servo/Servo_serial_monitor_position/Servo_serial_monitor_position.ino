#include <Servo.h>

#define SERVO_PIN 0

String inputString = "";
Servo servo;

int position = 90;
void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  servo.write(position);
}

void loop() {

   while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // Enter
      position = inputString.toInt();
      if (position >= 0 && position <= 180) {
        servo.write(position); // nastavi servo na uhol
        Serial.print("Servo je nastavny na uhli: ");
        Serial.println(position);
      } else {
        Serial.println("Neplatna pozicia");
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }

  delay(50);
}